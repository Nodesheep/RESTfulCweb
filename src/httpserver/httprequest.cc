#include "httprequest.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <algorithm>

namespace cweb {
namespace httpserver {

static std::string header_field_temp = "";

HttpRequestBody* HttpRequestBody::HttpRequestBodyFactory(const std::string &content_type) {
    if(content_type == "application/json") {
        return new HttpRequestJsonBody();
    }else if(content_type == "application/x-www-form-urlencoded") {
        return new HttpRequestFormBody();
    }else if (content_type.find("multipart/form-data") == 0) {
        return new HttpRequestMultipartBody(content_type);
    }else {
        return nullptr;
    }
}

void MultipartPart::SetData(const void *data, size_t size) {
    data_.reset(new BinaryData((const char*)data, size));
    size_ = size;
}

void MultipartPart::SetData(const StringPiece &data) {
    data_.reset(new BinaryData(data.Data(), data.Size()));
    size_ = data.Size();
}

void MultipartPart::SetFile(const std::string &filepath) {
    fd_ = open(filepath.c_str(), O_RDONLY);
    assert(fd_ > 0);
    struct stat st;
    fstat(fd_, &st);
    size_ = st.st_size;
}

void MultipartPart::SetFile(int fd, size_t size) {
    assert(fd > 0);
    fd_ = fd;
    size_ = size;
}

MultipartPart::MultipartPart() : data_(new BinaryData()) {}

bool MultipartPart::ParseBody() {
    auto iter = headers.find("Content-Type");
    if(iter == headers.end()) return true;
    content_type_ = iter->second;
    body_.reset(HttpRequestBody::HttpRequestBodyFactory(content_type_));
    if(body_ != nullptr) {
        return body_->SetData(data_->data, data_->size);
    }
    
    return true;
}

const Json::Value& MultipartPart::JsonValue() const {
    static Json::Value empty;
    if(content_type_ != "application/json") return empty;
    return std::dynamic_pointer_cast<HttpRequestJsonBody>(body_)->JsonValue();
}

const BinaryData& MultipartPart::BinaryValue() const {
    return *data_;
}

const std::string& MultipartPart::PostForm(const std::string &key) const {
    static std::string empty;
    if(content_type_ != "application/x-www-form-urlencoded") return empty;
    return std::dynamic_pointer_cast<HttpRequestFormBody>(body_)->FormData(key);
}

const std::string& MultipartPart::HeaderStr() {
    if(headerstr_.size() > 0) return headerstr_;
    for(auto iter = headers.begin(); iter != headers.end(); ++iter) {
        headerstr_ += iter->first + ": " + iter->second + "\r\n";
    }
    headerstr_ += "\r\n";
    return headerstr_;
}

bool HttpRequestFormBody::SetData(const char* data, size_t size) {
    const char* start = data;
    const char* end = data + size;
    const char* flag = nullptr;
    const char* equal = nullptr;
    do {
        flag = std::find(start, end, '&');
        equal = std::find(start, flag, '=');
        if(flag == end && equal == flag) {
            return false;
        }
        formdatas_.emplace(std::string(start, equal), std::string(equal + 1, flag));
        start = flag + 1;
    }while(flag != end);
    
    return true;
}

const std::string& HttpRequestFormBody::FormData(const std::string& key) const {
    auto iter = formdatas_.find(key);
    if(iter != formdatas_.end()) {
        return iter->second;
    }
    static std::string empty;
    return empty;
}

bool HttpRequestJsonBody::SetData(const char* data, size_t size) {
    Json::CharReaderBuilder crb;
    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
    return cr->parse(data, data + size, &json_, nullptr);
}

const Json::Value& HttpRequestJsonBody::JsonValue() const {
    return json_;
}

HttpRequestMultipartBody::HttpRequestMultipartBody(const std::string& content_type) : HttpRequestBody("multipart/form-data") {
    
    std::size_t pos = content_type.find("boundary=");
    if(pos != std::string::npos) {
        boundary_ = content_type.substr(pos + 9);
    }
    
    multipartparser_init(&parser_, boundary_.c_str());
    parser_.data = this;
    callbacks_.on_part_begin = handlePartBegin;
    callbacks_.on_header_field = handleHeaderField;
    callbacks_.on_header_value = handleHeaderValue;
    callbacks_.on_data = handleData;
    callbacks_.on_part_end = handlePartEnd;
}

HttpRequestMultipartBody::~HttpRequestMultipartBody() {
    for(auto part : multiparts_) {
        delete part;
    }
}

bool HttpRequestMultipartBody::SetData(const char* data, size_t size) {
    if(boundary_.size() == 0) return false;
    return multipartparser_execute(&parser_, &callbacks_, data, size) == size;
}

int HttpRequestMultipartBody::handlePartBegin(multipartparser *parser) {
    HttpRequestMultipartBody* body = (HttpRequestMultipartBody*)parser->data;
    MultipartPart* part = new MultipartPart();
    body->multiparts_.push_back(part);
    return 0;
}

int HttpRequestMultipartBody::handleHeaderField(multipartparser *parser, const char *data, size_t size) {
    header_field_temp.append(data, size);
    return 0;
}

int HttpRequestMultipartBody::handleHeaderValue(multipartparser *parser, const char *data, size_t size) {
    HttpRequestMultipartBody* body = (HttpRequestMultipartBody*)parser->data;
    if(header_field_temp == "Content-Disposition") {
        std::string disposition(data, size);
        std::size_t name_pos = disposition.find("name=\"");
        if(name_pos != std::string::npos) {
            name_pos += 6;
            body->multiparts_.back()->dispositions["name"] = disposition.substr(name_pos, disposition.find("\"", name_pos) - name_pos);
        }
        
        std::size_t filename_pos = disposition.find("filename=\"");
        if(filename_pos != std::string::npos) {
            filename_pos += 10;
            body->multiparts_.back()->dispositions["filename"] = disposition.substr(filename_pos, disposition.find("\"", filename_pos) - filename_pos);
        }
    }
    body->multiparts_.back()->headers[header_field_temp] = std::string(data, size);
    header_field_temp.clear();
    return 0;
}

int HttpRequestMultipartBody::handleData(multipartparser *parser, const char *data, size_t size) {
    HttpRequestMultipartBody* body = (HttpRequestMultipartBody*)parser->data;
    body->multiparts_.back()->SetData(data, size);
    return 0;
}

int HttpRequestMultipartBody::handlePartEnd(multipartparser *parser) {
    return 0;
}

MultipartPart* HttpRequestMultipartBody::MultipartForm(const std::string& key) const {
    static MultipartPart empty;
    for(auto part : multiparts_) {
        if(part->dispositions["name"] == key) {
            return part;
        }
    }
    return &empty;
}

HttpRequest::HttpRequest() : raw_body_(new ByteBuffer()) {}

bool HttpRequest::ParseBody() {
    data_.reset(new BinaryData(raw_body_->Peek(), raw_body_->ReadableBytes()));
    auto iter = headers_.find("Content-Type");
    if(iter == headers_.end()) return true;
    content_type_ = iter->second;
    body_.reset(HttpRequestBody::HttpRequestBodyFactory(content_type_));
    if(body_ != nullptr) {
        return body_->SetData(raw_body_->Peek(), raw_body_->ReadableBytes());
    }
    return true;
}

const std::string& HttpRequest::PostForm(const std::string &key) const {
    static std::string empty;
    if(content_type_ != "application/x-www-form-urlencoded") return empty;
    return std::dynamic_pointer_cast<HttpRequestFormBody>(body_)->FormData(key);
}

MultipartPart* HttpRequest::MultipartForm(const std::string& key) const {
    static MultipartPart empty;
    if(content_type_.find("multipart/form-data") != 0) return &empty;
    return std::dynamic_pointer_cast<HttpRequestMultipartBody>(body_)->MultipartForm(key);
}

const Json::Value& HttpRequest::JsonValue() const {
    static Json::Value empty;
    if(content_type_ != "application/json") return empty;
    return std::dynamic_pointer_cast<HttpRequestJsonBody>(body_)->JsonValue();
}

const BinaryData& HttpRequest::BinaryValue() const {
    return *data_;
}

}
}
