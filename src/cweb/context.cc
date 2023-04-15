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
    std::stringstream* content = new std::stringstream();
    HttpResponse::SetStatusCode(code, content);
    HttpResponse::SetHeader("Content-Type", "text/html; charset=utf-8", content);
    HttpResponse::SetHeader("Content-Length", std::to_string(data.Size()), content);
    HttpResponse::SetBody(data, content);
    
    connection_->Send(content);
}

void Context::JSON(HttpStatusCode code, Json::Value value) {
    std::ostringstream jsonos;
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["emitUTF8"] = true;
    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(value, &jsonos);
    std::string jsonstr = jsonos.str();
    
    std::stringstream* content = new std::stringstream();
    HttpResponse::SetStatusCode(code, content);
    HttpResponse::SetHeader("Content-Type", "application/json; charset=utf-8", content);
    HttpResponse::SetHeader("Content-Length", std::to_string(jsonstr.size()), content);
    HttpResponse::SetBody(jsonstr, content);
    connection_->Send(content);
}

void Context::FILE(HttpStatusCode code, const std::string &filepath) {
    
}

}
