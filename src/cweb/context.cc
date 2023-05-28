#include "context.h"
#include "httpserver.h"
#include "httpsession.h"
#include "bytedata.h"
#include <random>
#include <fstream>

namespace cweb {

Context::~Context() {
    if(redis_) {
        RedisPoolSingleton::GetInstance()->ReleaseConnection(redis_);
    }
    
    if(mysql_) {
        MySQLPoolSingleton::GetInstance()->ReleaseConnection(mysql_);
    }
}

const std::string& Context::Method() const {
    return request_->Method();
}

const std::string& Context::Path() const {
    return request_->Path();
}

const std::string& Context::Param(const std::string &key) {
    static std::string empty;
    auto iter = params_.find(key);
    if(iter != params_.end()) {
        return iter->second;
    }
    return empty;
}

const std::string& Context::Query(const std::string &key) const {
    return request_->Query(key);
}

const std::string& Context::PostForm(const std::string &key) const {
    return request_->PostForm(key);
}

MultipartPart* Context::MultipartForm(const std::string& key) const {
    return request_->MultipartForm(key);
}

const Json::Value& Context::JsonValue() const {
    return request_->JsonValue();
}

const BinaryData& Context::BinaryValue() const {
    return request_->BinaryValue();
}

void Context::SaveUploadedFile(const BinaryData &file, const std::string &path, const std::string& filename) {
    std::ofstream ofs;
    ofs.open(path + filename, std::ofstream::out | std::ofstream::app);
    ofs.write(file.data, file.size);
    ofs.close();
}

std::shared_ptr<Redis> Context::Redis() {
    if(redis_) return redis_;
    redis_ = RedisPoolSingleton::GetInstance()->GetConnection();
    return redis_;
}

std::shared_ptr<MySQL> Context::MySQL() {
    if(mysql_) return mysql_;
    mysql_ = MySQLPoolSingleton::GetInstance()->GetConnection();
    return mysql_;
}

std::shared_ptr<WebSocket> Context::Upgrade() const {
    return std::dynamic_pointer_cast<WebSocket>(session_);
}

void Context::STRING(HttpStatusCode code, const std::string& data) {
    session_->SendString(code, data);
}

void Context::JSON(HttpStatusCode code, const std::string& data) {
    session_->SendJson(code, data);
}

//单文件传输
void Context::FILE(HttpStatusCode code, const std::string &filepath, std::string filename) {
    session_->SendFile(code, filepath, filename);
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
void Context::MULTIPART(HttpStatusCode code, const std::vector<MultipartPart *>& parts) {
    session_->SendMultipart(code, parts);
}


}
