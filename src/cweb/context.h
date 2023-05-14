#ifndef CWEB_CWEB_CONTEXT_H_
#define CWEB_CWEB_CONTEXT_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "router.h"
#include "bytebuffer.h"
#include "http_code.h"
#include "httpserver.h"
#include "httprequest.h"
#include "websocket.h"

using namespace cweb::tcpserver;
using namespace cweb::httpserver;
using namespace cweb::util;

namespace cweb {

class Context : public std::enable_shared_from_this<Context> {
private:
    int index_ = -1;
    std::vector<ContextHandler> handlers_;
    std::unique_ptr<HttpRequest> request_;
    //shared 避免下层通道关闭后 上层无感知导致send空指针
    std::shared_ptr<HttpSession> session_;
    std::unordered_map<std::string, std::string> params_;
    
public:
    friend class Router;
    Context(std::shared_ptr<HttpSession> session, std::unique_ptr<HttpRequest> req) : session_(session), request_(std::move(req)) {}
    
    void Next() {
        ++index_;
        if(index_ < handlers_.size()) {
            (handlers_[index_])(shared_from_this());
        }
    }
    
    void AddHandler(ContextHandler handler) {
        handlers_.push_back(std::move(handler));
    }
    
    const std::string& Method() const;
    const std::string& Path() const;
    const std::string& Query(const std::string& key) const;
    const std::string& Param(const std::string& key);
    const std::string& PostForm(const std::string& key) const;
    MultipartPart* MultipartForm(const std::string& key) const;
    const Json::Value& JsonValue() const;
    const BinaryData& BinaryValue() const;
    
    std::shared_ptr<WebSocket> Upgrade() const;

    void SaveUploadedFile(const BinaryData& file, const std::string& path, const std::string& filename);
    
    void STRING(HttpStatusCode code, const std::string& data);
    void JSON(HttpStatusCode code, const std::string& data);
    void FILE(HttpStatusCode code, const std::string& filepath, std::string filename = "");
    void MULTIPART(HttpStatusCode code, const std::vector<MultipartPart*>& parts);
};

}
#endif /* context_hpp */
