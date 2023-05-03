#include "context.h"
#include "httpserver.h"
#include "bytedata.h"
#include <random>
#include <fcntl.h>
#include <sys/stat.h>

namespace cweb {

void MultipartPart::SetData(const StringPiece &data) {
    data_ = data.Data();
    size_ = data.Size();
    headers_ += "Content-Length: " + std::to_string(size_) + "\r\n\r\n";
}

void MultipartPart::SetData(const void *data, size_t size) {
    data_ = (const char*)data;
    size_ = size;
    headers_ += "Content-Length: " + std::to_string(size_) + "\r\n\r\n";
}

void MultipartPart::SetFile(const std::string &filepath) {
    fd_ = open(filepath.c_str(), O_RDONLY);
    assert(fd_ > 0);
    struct stat st;
    fstat(fd_, &st);
    size_ = st.st_size;
    headers_ += "Content-Length: " + std::to_string(size_) + "\r\n\r\n";
}

void MultipartPart::SetFile(int fd, size_t size) {
    assert(fd > 0);
    fd_ = fd;
    size_ = size;
    headers_ += "Content-Length: " + std::to_string(size_) + "\r\n\r\n";
}

std::string Context::Method() const {
    return request_->method_;
}

std::string Context::Path() const {
    return request_->path_;
}

std::string Context::Param(const std::string &key) {
    return params_[key];
}

std::string Context::Query(const std::string &key) const {
    return request_->querys_[key];
}

std::string Context::PostForm(const std::string &key) const {
    return request_->PostForm(key);
}

void Context::STRING(HttpStatusCode code, const std::string& data) {
    std::string header;
    HttpResponse::SetStatusCode(code, header);
    HttpResponse::SetHeader("Content-Type", "text/html; charset=utf-8", header);
    HttpResponse::SetHeader("Content-Length", std::to_string(data.size()), header);
    header += "\r\n";
    
    util::ByteData* bdata = new util::ByteData();
    bdata->AddDataZeroCopy(header);
    bdata->AddDataZeroCopy(data);
    connection_->Send(bdata);
}

void Context::JSON(HttpStatusCode code, const std::string& data) {
    std::string header;
    HttpResponse::SetStatusCode(code, header);
    HttpResponse::SetHeader("Content-Type", "application/json; charset=utf-8", header);
    HttpResponse::SetHeader("Content-Length", std::to_string(data.size()), header);
    //HttpResponse::SetBody(jsonstr, content);
    header += "\r\n";
    
    util::ByteData* bdata = new util::ByteData();
    bdata->AddDataZeroCopy(header);
    bdata->AddDataZeroCopy(data);
    connection_->Send(bdata);
}

//单文件传输
void Context::FILE(HttpStatusCode code, const std::string &filepath, std::string filename) {
    if(filename.size() == 0) {
        size_t pos = filepath.find_last_of('/');
        if(pos != std::string::npos) {
            filename = filepath.substr(pos + 1);
        }
    }
    
    std::string header;
    HttpResponse::SetStatusCode(code, header);
    HttpResponse::SetHeader("Content-Type", "application/octet-stream", header);
    HttpResponse::SetHeader("Content-Disposition", "attachment; filename=" + filename, header);
    int fd = open(filepath.c_str(), O_RDONLY);
    assert(fd > 0);
    struct stat st;
    fstat(fd, &st);
    HttpResponse::SetHeader("Content-Length", std::to_string(st.st_size), header);
    header += "\r\n";
    
    util::ByteData* bdata = new util::ByteData();
    bdata->AddDataZeroCopy(header);
    bdata->AddFile(fd, st.st_size);
    
    connection_->Send(bdata);
}

//MULTIPART 数据
/*
POST /upload HTTP/1.1
Host: example.com
Content-Type: multipart/form-data; boundary=----BOUNDARY_STRING

------BOUNDARY_STRING
Content-Disposition: form-data; name="file"; filename="test.jpg"
Content-Type: image/jpeg

<JPEG file data>
------BOUNDARY_STRING--*/
void Context::MULTIPART(HttpStatusCode code, std::vector<MultipartPart *> parts) {
    std::string boundary = generateBoundary(16);
    std::string header;
    HttpResponse::SetStatusCode(code, header);
    HttpResponse::SetHeader("Content-Type", "multipart/form-data; boundary=" + boundary, header);
    
    std::string begin_boundary = "\r\n--" + boundary + "\r\n";
    std::string end_boundary = "\r\n--" + boundary + "--";

    int totalsize = 0;
    for(MultipartPart* part : parts) {
        totalsize += begin_boundary.size() + part->headers_.size() + part->size_;
    }
    totalsize += end_boundary.size() - 2;
    HttpResponse::SetHeader("Content-Length", std::to_string(totalsize), header);
    
    
    util::ByteData* bdata = new util::ByteData();
    bdata->AddDataZeroCopy(header);
    for(MultipartPart* part : parts) {
        bdata->AddDataZeroCopy(begin_boundary);
        bdata->AddDataZeroCopy(part->headers_);
        if(part->fd_ > 0) {
            bdata->AddFile(part->fd_, part->size_);
        }else {
            bdata->AddDataCopy(part->data_, part->size_);
        }
    }
    bdata->AddDataZeroCopy(end_boundary);
    connection_->Send(bdata);
}

std::string Context::generateBoundary(size_t len) {
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
