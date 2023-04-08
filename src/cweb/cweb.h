#ifndef CWEB_CWEB_CWEB_H_
#define CWEB_CWEB_CWEB_H_

#include <vector>
#include "router.h"
#include "connection.h"

using namespace cweb::tcpserver;

namespace cweb {

class HttpServer;
class HttpRequest;
class Group {
private:
    const std::string prefix_;
    std::vector<ContextHandler> middlewares_;
    Router* router_ = nullptr;
    
public:
    friend class Cweb;
    Group(const std::string& prefix, Router* router) : prefix_(prefix), router_(router) {}
    
    void Use(ContextHandler handler) {
        middlewares_.push_back(std::move(handler));
    }
    
    void GET(const std::string& path, ContextHandler handler) {
        router_->AddRouter("GET", prefix_ + path, handler);
    }
    
    void POST(const std::string& path, ContextHandler handler) {
        router_->AddRouter("POST", prefix_ + path, handler);
    }
    
    void DELETE(const std::string& path, ContextHandler handler) {
        router_->AddRouter("DELETE", prefix_ + path, handler);
    }
};

class Cweb {
private:
    Router* router_ = nullptr;
    std::vector<ContextHandler> global_handlers_;
    std::vector<Group*> groups_;
    HttpServer* httpserver_ = nullptr;
    
    void serverHTTP(Connection* conn, HttpRequest* req);
    
public:
    Cweb(uint16_t port, bool loopbackonly = false, bool ipv6 = false);
    Cweb(const std::string& ip, uint16_t port, bool ipv6 = false);
    
    ~Cweb();
    
    void Use(ContextHandler handler) {
        global_handlers_.push_back(std::move(handler));
    }
    
    void GET(const std::string& path, ContextHandler handler) {
        router_->AddRouter("GET", path, handler);
    }
    
    void POST(const std::string& path, ContextHandler handler) {
        router_->AddRouter("POST", path, handler);
    }
    
    void DELETE(const std::string& path, ContextHandler handler) {
        router_->AddRouter("DELETE", path, handler);
    }
    
    class Group* Group(const std::string& prefix) {
        class Group* group = new class Group(prefix, router_);
        groups_.push_back(std::move(group));
        return group;
    }
    
    void Run(int threadcnt);
    void Quit();
};

}

#endif
