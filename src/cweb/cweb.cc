#include "cweb.h"
#include "context.h"
#include "httpserver.h"
#include "redis.h"
#include "mysql.h"
#include "logger.h"
#ifdef COROUTINE
#include "co_eventloop.h"
#else
#include "eventloop.h"
#endif

#ifdef COROUTINE
using namespace cweb::tcpserver::coroutine;
#endif

using namespace cweb::db;
using namespace cweb::log;

namespace cweb {

#ifdef COROUTINE
std::shared_ptr<EventLoop> mainloop(new CoEventLoop());
#else
std::shared_ptr<EventLoop> mainloop(new EventLoop());
#endif

Cweb::Cweb(uint16_t port, bool loopbackonly, bool ipv6) {
    LoggerManagerSingleton::GetInstance();
    if(!RedisPoolSingleton::GetInstance()->Init()) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "cweb", "redis init error");
    }
    if(!MySQLPoolSingleton::GetInstance()->Init()) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "cweb", "mysql init error");
    }
    LoggerManagerSingleton::GetInstance();
    router_.reset(new Router());
    httpserver_.reset(new HttpServer(mainloop, port, loopbackonly, ipv6));
    httpserver_->SetRequestCallback(std::bind(&Cweb::serverHTTP, this, std::placeholders::_1, std::placeholders::_2));
}

Cweb::Cweb(const std::string& ip, uint16_t port, bool ipv6) {
    LoggerManagerSingleton::GetInstance();
    if(!RedisPoolSingleton::GetInstance()->Init()) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "cweb", "redis init error");
    }
    if(!MySQLPoolSingleton::GetInstance()->Init()) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "cweb", "mysql init error");
    }
    router_.reset(new Router());
    httpserver_.reset(new HttpServer(mainloop, ip, port, ipv6));
    httpserver_->SetRequestCallback(std::bind(&Cweb::serverHTTP, this, std::placeholders::_1, std::placeholders::_2));
}

Cweb::~Cweb() {
    for(class Group* group : groups_) {
        delete group;
    }
}

void Cweb::serverHTTP(std::shared_ptr<HttpSession> session, std::unique_ptr<HttpRequest> req) {
    std::shared_ptr<Context> c = std::make_shared<Context>(session, std::move(req));
    
    //全局中间件
    for(ContextHandler handler : global_handlers_) {
        //验证有无问题
        c->AddHandler(std::move(handler));
    }
    
    for(class Group* group : groups_) {
        if(c->Path().find(group->prefix_) == 0) {
            for(ContextHandler handler : group->middlewares_) {
                c->AddHandler(std::move(handler));
            }
        }
    }
    
    router_->Handle(c);
}

void Cweb::Run(int threadcnt) {
    LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "cweb", "server start success");
    httpserver_->Start(threadcnt);
}

void Cweb::Quit() {
    httpserver_->Quit();
}

}
