#ifndef CWEB_CWEB_CONTEXT_H_
#define CWEB_CWEB_CONTEXT_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "router.h"
#include "connection.h"
#include "bytebuffer.h"
#include "http_code.h"
#include "json.h"
#include "httpserver.h"

using namespace cweb::tcpserver;
using namespace cweb::util;

namespace cweb {

class HttpRequest;
class Context {
private:
    int index_ = -1;
    std::vector<ContextHandler> handlers_;
    HttpRequest* request_;
    Connection* connection_;
    std::unordered_map<std::string, std::string> params_;
    
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
    
    void STRING(HttpStatusCode code, StringPiece data);
    void JSON(HttpStatusCode code, Json::Value value);
    //void HTML();
    void FILE(HttpStatusCode code, const std::string& filepath);
};

}
#endif /* context_hpp */
