#ifndef CWEB_CWEB_CONTEXT_H_
#define CWEB_CWEB_CONTEXT_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "router.h"
#include "connection.h"
#include "bytebuffer.h"
#include "http_code.h"
#include "httpserver.h"

using namespace cweb::tcpserver;
using namespace cweb::util;

namespace cweb {

//TODO 临时设计 后续优化
class MultipartPart {
private:
    std::string headers_;
    const char* data_ = nullptr;
    int fd_ = -1;
    size_t size_ = 0;

public:
    friend class Context;
    void SetHeader(const std::string& key, const std::string& value) {
        headers_ += key + ": " + value + "\r\n";
    }
    
    void SetData(const StringPiece& data);
    void SetData(const void* data, size_t size);
    
    void SetFile(const std::string& filepath);
    void SetFile(int fd, size_t size);
};

class HttpRequest;
class Context {
private:
    int index_ = -1;
    std::vector<ContextHandler> handlers_;
    HttpRequest* request_;
    Connection* connection_;
    std::unordered_map<std::string, std::string> params_;
    
    std::string generateBoundary(size_t len);
    
public:
    friend class Router;
    Context(Connection* conn, HttpRequest* req) : connection_(conn), request_(req) {}
    ~Context() {delete request_;}
    
    void Next() {
        ++index_;
        if(index_ < handlers_.size()) {
            (handlers_[index_])(this);
        }
    }
    
    void AddHandler(ContextHandler handler) {
        handlers_.push_back(std::move(handler));
    }
    
    std::string Method() const;
    std::string Path() const;
    std::string Query(const std::string& key) const;
    std::string Param(const std::string& key);
    std::string PostForm(const std::string& key) const;
    //TODO
    //FILE FormFile(const std::string& filename);
    
    void STRING(HttpStatusCode code, const StringPiece& data);
    void JSON(HttpStatusCode code, const StringPiece& data);
    void FILE(HttpStatusCode code, const std::string& filepath, std::string filename = "");
    
    void MULTIPART(HttpStatusCode code, std::vector<MultipartPart*> parts);
    
    void SaveUploadedFile(File file, const std::string& path);
};

}
#endif /* context_hpp */
