#include "httpsession.h"
#include "websocket.h"
#include "httpresponse.h"
#include <fcntl.h>
#include <random>
#include <sys/stat.h>
#include <algorithm>

namespace cweb {
namespace httpserver {

HttpSession::HttpSession(std::shared_ptr<TcpConnection> conn, RequestCallback cb) : connection_(conn), request_callback_(cb) {}

HttpSession::~HttpSession() {
    if(upgrade_) {
        websocket_->HandleClose();
    }
}

void HttpSession::Init() {
    http_parser_.reset(new HttpParser(shared_from_this()));
}

TcpConnection::MessageState HttpSession::handleMessage(std::shared_ptr<TcpConnection> conn, ByteBuffer* buf, Time time) {
    TcpConnection::MessageState state;
    if(upgrade_) {
        state = websocket_->handleMessage(conn, buf, time);
    }else {
        state = (TcpConnection::MessageState)http_parser_->Parse(buf->Peek(), buf->ReadableBytes());
        buf->ReadAll();
    }
    
    if(state == tcpserver::TcpConnection::BAD) {
        connection_->ForceClose();
    }
    return state;
}

void HttpSession::handleParsedMessage(std::unique_ptr<HttpRequest> request) {
    if(http_parser_->IsUpgrade()) {
        upgrade_ = true;
        websocket_.reset(new WebSocket(connection_, request_callback_));
        websocket_->Start(std::move(request));
        return;
    }
    
    auto iter = request->headers_.find("connection");
    if(http_parser_->CheckVersion(1, 0)) {
        if(iter != request->headers_.end() && iter->second == "Keep-Alive") {
            //长连
            need_close_ = false;
        }
    }else if(http_parser_->CheckVersion(1, 1)) {
        
        if(iter == request->headers_.end() || iter->second == "close") {
            need_close_ = false;
        }
    }
    if(request->ParseBody()) {
        request_callback_(shared_from_this(), std::move(request));
        //TODO 线程版 业务不能异步处理 协程版可以挂起任务
        if(need_close_) {
            connection_->ForceClose();
        }
    }
}

void HttpSession::SendString(HttpStatusCode code, const std::string& data) {
    std::string header;
    HttpResponse::SetStatusCode(code, header);
    HttpResponse::SetHeader("Content-Type", "text/plain; charset=utf-8", header);
    HttpResponse::SetHeader("Content-Length", std::to_string(data.size()), header);
    header += "\r\n";
    
    ByteData* bdata = new ByteData();
    bdata->AddDataZeroCopy(header);
    bdata->AddDataZeroCopy(data);
    Send(bdata);
}

void HttpSession::SendJson(HttpStatusCode code, const std::string& data) {
    std::string header;
    HttpResponse::SetStatusCode(code, header);
    HttpResponse::SetHeader("Content-Type", "application/json; charset=utf-8", header);
    HttpResponse::SetHeader("Content-Length", std::to_string(data.size()), header);
    //HttpResponse::SetBody(jsonstr, content);
    header += "\r\n";
    
    ByteData* bdata = new ByteData();
    bdata->AddDataZeroCopy(header);
    bdata->AddDataZeroCopy(data);
    Send(bdata);
}

void HttpSession::SendFile(HttpStatusCode code, const std::string& filepath, std::string filename) {
    static std::unordered_map<std::string, std::string> filetypes = {
        {"jpeg", "image/jpeg"},
        {"jpg", "image/jpeg"},
        {"png", "image.png"},
        {"gif", "image/gif"},
        {"mp3", "audio/mpeg"},
        {"mp4", "video/mp4"},
        {"txt", "text/plain"},
        {"html", "text/html"},
        {"pdf", "application/pdf"},
        {"doc", "application/msword"},
        {"docx", "application/msword"},
        {"zip", "application/zip"},
        {"gzip", "application/gzip"}
    };
    
    if(filename.size() == 0) {
        size_t pos = filepath.find_last_of('/');
        if(pos != std::string::npos) {
            filename = filepath.substr(pos + 1);
        }
    }
    
    std::string filetype = filepath.substr(filepath.find_last_of(".") + 1);
    std::transform(filetype.begin(), filetype.end(), filetype.begin(), ::tolower);
    
    auto iter = filetypes.find(filetype);
    if(iter == filetypes.end()) {
        filetype = "application/octet-stream";
    }else {
        filetype = iter->second;
    }
    
    std::string header;
    HttpResponse::SetStatusCode(code, header);
    HttpResponse::SetHeader("Content-Type", filetype, header);
    HttpResponse::SetHeader("Content-Disposition", "attachment; filename=" + filename, header);
    int fd = open(filepath.c_str(), O_RDONLY);
    assert(fd > 0);
    struct stat st;
    fstat(fd, &st);
    HttpResponse::SetHeader("Content-Length", std::to_string(st.st_size), header);
    header += "\r\n";
    
    ByteData* bdata = new ByteData();
    bdata->AddDataZeroCopy(header);
    bdata->AddFile(fd, st.st_size);
    Send(bdata);
}

void HttpSession::SendMultipart(HttpStatusCode code, const std::vector<MultipartPart*>& parts) {
    
    std::string boundary = generateBoundary(16);
    std::string header;
    HttpResponse::SetStatusCode(code, header);
    HttpResponse::SetHeader("Content-Type", "multipart/form-data; boundary=" + boundary, header);
    
    std::string begin_boundary = "\r\n--" + boundary + "\r\n";
    std::string end_boundary = "\r\n--" + boundary + "--";

    int totalsize = 0;
    for(MultipartPart* part : parts) {
        totalsize += begin_boundary.size() + part->HeaderStr().size() + part->Size();
    }
    totalsize += end_boundary.size() - 2;
    HttpResponse::SetHeader("Content-Length", std::to_string(totalsize), header);
    
    
    ByteData* bdata = new ByteData();
    bdata->AddDataZeroCopy(header);
    for(MultipartPart* part : parts) {
        bdata->AddDataZeroCopy(begin_boundary);
        bdata->AddDataZeroCopy(part->HeaderStr());
        if(part->Fd() > 0) {
            bdata->AddFile(part->Fd(), part->Size());
        }else {
            bdata->AddDataZeroCopy(part->Data(), part->Size());
        }
    }
    bdata->AddDataZeroCopy(end_boundary);
    Send(bdata);
}

void HttpSession::Send(ByteData* data) {
    connection_->Send(data);
}

std::string HttpSession::generateBoundary(size_t len) {
    static std::string charset = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static int length = (int)charset.size();
    std::string boundary = "----";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, length - 1);
    for(int i = 0; i < len; ++i) {
        boundary += charset[dis(gen)];
    }
    return boundary;
}


}
}
