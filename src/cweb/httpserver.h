#ifndef CWEB_CWEB_HTTPSERVER_H_
#define CWEB_CWEB_HTTPSERVER_H_

#include <functional>
#include <unordered_map>
#include <string>
#include "http_parser.h"
#include "timer.h"
#include "bytebuffer.h"
#include "server.h"
#include "connection.h"
#include "eventloop.h"
#include "http_code.h"

using namespace cweb::util;
using namespace cweb::tcpserver;

namespace cweb {

class HttpRequestBody {
protected:
    const std::string content_type_;

public:
    virtual ~HttpRequestBody() {}
    friend class HttpRequest;
    HttpRequestBody(const std::string& type) : content_type_(type) {}
    virtual bool SetData(ByteBuffer* data) = 0;
};

class HttpRequestFormBody : public HttpRequestBody {
private:
    std::unordered_map<std::string, std::string> formdatas_;
    
public:
    virtual ~HttpRequestFormBody() {}
    HttpRequestFormBody() : HttpRequestBody("application/x-www-form-urlencoded") {}
    virtual bool SetData(ByteBuffer* data) override;
    std::string FormData(const std::string& key);
};

class HttpRequestJsonBody : public HttpRequestBody {
private:
    std::string json_string_;
    
public:
    virtual ~HttpRequestJsonBody() {}
    HttpRequestJsonBody() : HttpRequestBody("application/json") {}
    virtual bool SetData(ByteBuffer* data) override;
};

class HttpRequest {
private:
    enum ParserProcess {
        INIT,
        PROCESS,
        SUCCESS,
        FAIL
    };
    
    std::string method_;
    std::unordered_map<std::string, std::string> headers_;
    std::string url_ = "";
    std::string path_ = "";
    std::string header_key_temp_ = "";
    std::unordered_map<std::string, std::string> querys_;
    ByteBuffer* binary_body_ = nullptr;
    HttpRequestBody* body_ = nullptr;
    
    http_parser* parser_ = nullptr;
    http_parser_settings* parser_settings_ = nullptr;

    ParserProcess parser_process_ = INIT;
    Timer* remove_timer_ = nullptr;
    
    
    static int handleMessageBegin(http_parser* parser);
    static int handleURL(http_parser* parser, const char *at, size_t length);
    static int handleHeaderField(http_parser* parser, const char *at, size_t length);
    static int handleHeaderValue(http_parser* parser, const char *at, size_t length);
    static int handleBody(http_parser* parser, const char *at, size_t length);
    static int handleMessageComplete(http_parser* parser);
    
public:
    friend class HttpServer;
    friend class Context;
    HttpRequest();
    ~HttpRequest();
    int Parse(const void* data, size_t len);
    
    std::string PostForm(const std::string &key);
};

class HttpResponse {
public:
    HttpResponse();
    ~HttpResponse();
    static void SetHeader(const std::string& key, const std::string& value, std::string& stream);
    static void SetStatusCode(HttpStatusCode code, std::string& stream);
    static void SetBody(StringPiece body, std::string& stream);
};

class HttpServer {
private:
    typedef std::function<void(Connection*, HttpRequest*)> RequestCallback;
    Server* tcpserver_ = nullptr;
    //一个连接对应一个请求/短链接
    std::unordered_map<Connection*, HttpRequest*> conn_req_;
    //一个连接对应多个请求/长连接
    std::unordered_map<Connection*, std::unordered_map<std::string, HttpRequest*>> conn_reqs_;
    RequestCallback request_callback_;
    
    void handleMessage(Connection* conn, ByteBuffer* buf, Time time);
    void handleKeepAliveMessage(Connection* conn, ByteBuffer* buf, Time time);
    void removeRequest(Connection* conn);
    
public:
    HttpServer(EventLoop* loop, uint16_t port = 0, bool loopbackonly = false, bool ipv6 = false);
    HttpServer(EventLoop* loop, const std::string& ip, uint16_t port, bool ipv6 = false);
    
    ~HttpServer();
    
    void Start(int threadcnt);
    void Quit();
    void SetRequestCallback(RequestCallback cb) {request_callback_ = std::move(cb);}
};

}

#endif

