#ifndef CWEB_TCP_HTTPSERVER_H_
#define CWEB_TCP_HTTPSERVER_H_

#include <functional>
#include <unordered_map>
#include <string>
#include "http_parser.h"
#include "../tcpserver/timer.h"
#include "../tcpserver/tcpconnection.h"
#include "../util/bytebuffer.h"

using namespace cweb::util;
using namespace cweb::tcpserver;

namespace cweb {
namespace http {

class HttpRequest {
private:
    std::unordered_map<std::string, std::string> headers_;
    std::string path_;
    std::unordered_map<std::string, std::string> querys_;
    ByteBuffer* body_;
    
    http_parser* parser_;
    Timer* remove_timer_ = nullptr;
    
    
    static int handleMessageBegin(http_parser* parser);
    static int handleURL(http_parser* parser, const char *at, size_t length);
    static int handleHeaderField(http_parser* parser, const char *at, size_t length);
    static int handleHeaderValue(http_parser* parser, const char *at, size_t length);
    static int handleBody(http_parser* parser, const char *at, size_t length);
    static int handleMessageComplete(http_parser* parser);
    
public:
    friend class HttpServer;
    int Parse(const void* data, size_t len);
};

class HttpReponse {
    
};

class HttpServer {
private:
    typedef std::function<void(TcpConnection*, HttpRequest*)> RequestCallback;
    //一个连接对应一个请求/短链接
    std::unordered_map<TcpConnection*, HttpRequest*> conn_req_;
    //一个连接对应多个请求/长连接
    std::unordered_map<TcpConnection*, std::unordered_map<const std::string, HttpRequest*>> conn_reqs_;
    RequestCallback request_callback_;
    
    void handleMessage(TcpConnection* conn, ByteBuffer* buf, Time time);
    void handleKeepAliveMessage(TcpConnection* conn, ByteBuffer* buf, Time time);
    void removeRequest(TcpConnection* conn);
    
public:
    void SetRequestCallback(RequestCallback cb) {request_callback_ = std::move(cb);}
};

}
}

#endif

