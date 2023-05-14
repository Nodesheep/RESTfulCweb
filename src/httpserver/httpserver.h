#ifndef CWEB_HTTP_HTTPSERVER_H_
#define CWEB_HTTP_HTTPSERVER_H_

#include <functional>
#include <unordered_map>
#include <string>
#include "http_parser.h"
#include "timer.h"
#include "bytebuffer.h"
#include "tcpserver.h"
#include "tcpconnection.h"
#include "eventloop.h"
#include "http_code.h"

using namespace cweb::util;
using namespace cweb::tcpserver;

namespace cweb {
namespace httpserver {

class HttpSession;
class HttpRequest;
class HttpServer {
private:
    typedef std::function<void(std::shared_ptr<HttpSession>, std::unique_ptr<HttpRequest>)> RequestCallback;
    std::unique_ptr<TcpServer> tcpserver_;
    std::unordered_map<std::shared_ptr<TcpConnection>, std::shared_ptr<HttpSession>> httpsessions_;
    RequestCallback request_callback_;
    std::atomic_flag lock_ = ATOMIC_FLAG_INIT;
    
    void handleConnected(std::shared_ptr<TcpConnection> conn);
    void handleDisconnected(std::shared_ptr<TcpConnection> conn);
    
public:
    HttpServer(std::shared_ptr<EventLoop> loop, uint16_t port = 0, bool loopbackonly = false, bool ipv6 = false);
    HttpServer(std::shared_ptr<EventLoop> loop, const std::string& ip, uint16_t port, bool ipv6 = false);
    
    ~HttpServer();
    
    void Start(int threadcnt);
    void Quit();
    void SetRequestCallback(RequestCallback cb) {request_callback_ = std::move(cb);}
};

}
}

#endif

