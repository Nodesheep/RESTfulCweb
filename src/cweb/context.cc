#include "context.h"
#include "httpserver.h"

namespace cweb {

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

void Context::STRING(HttpStatusCode code, StringPiece data) {
    HttpReponse response;
    response.SetStatusCode(code);
    response.SetHeader("Content-Type", "text/html; charset=utf-8");
    response.SetHeader("Content-Length", std::to_string(data.Size()));
    response.SetBody(data);
    
    connection_->Send(response.SendData());
}

}
