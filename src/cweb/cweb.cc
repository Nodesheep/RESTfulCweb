#include "cweb.h"
#include "context.h"
#include "httpserver.h"
#ifdef COROUTINE
#include "co_eventloop.h"
#else
#include "eventloop.h"
#endif

#ifdef COROUTINE
using namespace cweb::tcpserver::coroutine;
#endif

namespace cweb {

#ifdef COROUTINE
CoEventLoop mainloop;
#else
EventLoop mainloop;
#endif

Cweb::Cweb(uint16_t port, bool loopbackonly, bool ipv6) {
    router_ = new Router();
    httpserver_ = new HttpServer(&mainloop, port, loopbackonly, ipv6);
    httpserver_->SetRequestCallback(std::bind(&Cweb::serverHTTP, this, std::placeholders::_1, std::placeholders::_2));
}

Cweb::Cweb(const std::string& ip, uint16_t port, bool ipv6) {
    router_ = new Router();
    httpserver_ = new HttpServer(&mainloop, ip, port, ipv6);
    httpserver_->SetRequestCallback(std::bind(&Cweb::serverHTTP, this, std::placeholders::_1, std::placeholders::_2));
}

Cweb::~Cweb() {
    //有问题 线程join时被销毁
    delete httpserver_;
    delete router_;
    for(class Group* group : groups_) {
        delete group;
    }
}

void Cweb::serverHTTP(Connection* conn, HttpRequest* req) {
    Context* c = new Context(conn, req);
    
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
    delete c;
}

void Cweb::Run(int threadcnt) {
    httpserver_->Start(threadcnt);
}

void Cweb::Quit() {
    httpserver_->Quit();
}

}
