#include "httpserver.h"
#include "tcpconnection.h"
#include "logger.h"
#include "http_parser.h"
#include "httpsession.h"
#ifdef COROUTINE
#include "co_tcpserver.h"
#else
#include "tcpserver.h"
#endif

#ifdef COROUTINE
using namespace cweb::tcpserver::coroutine;
#endif

using namespace cweb::log;

namespace cweb {
namespace httpserver {

HttpServer::HttpServer(std::shared_ptr<EventLoop> loop, uint16_t port, bool loopbackonly, bool ipv6) {
#ifdef COROUTINE
    tcpserver_.reset(new CoTcpServer(loop, port, loopbackonly, ipv6));
#else
    tcpserver_.reset(new TcpServer(loop, port, loopbackonly, ipv6));
#endif
    tcpserver_->SetConnectedCallback(std::bind(&HttpServer::handleConnected, this, std::placeholders::_1));
}

HttpServer::HttpServer(std::shared_ptr<EventLoop> loop, const std::string& ip, uint16_t port, bool ipv6) {
#ifdef COROUTINE
    tcpserver_.reset(new CoTcpServer(loop, ip, port, ipv6));
#else
    tcpserver_.reset(new TcpServer(loop, ip, port, ipv6));
#endif
    tcpserver_->SetConnectedCallback(std::bind(&HttpServer::handleConnected, this, std::placeholders::_1));
}

HttpServer::~HttpServer() {}

void HttpServer::Start(int threadcnt) {
    tcpserver_->Start(threadcnt);
}

void HttpServer::Quit() {
    tcpserver_->Quit();
}

void HttpServer::handleConnected(std::shared_ptr<TcpConnection> conn) {
    if(conn->Connected()) {
        std::shared_ptr<HttpSession> session(new HttpSession(conn, request_callback_));
        session->Init();
        conn->SetMessageCallback(std::bind(&HttpSession::handleMessage, session.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        while(lock_.test_and_set(std::memory_order_acquire)) {}
        httpsessions_[conn] = session;
        lock_.clear(std::memory_order_release);
    }else {
        handleDisconnected(conn);
    }
}

void HttpServer::handleDisconnected(std::shared_ptr<TcpConnection> conn) {
    auto iter = httpsessions_.find(conn);
    if(iter != httpsessions_.end()) {
        while(lock_.test_and_set(std::memory_order_acquire)) {}
        httpsessions_.erase(conn);
        lock_.clear(std::memory_order_release);
    }
}

}
}
