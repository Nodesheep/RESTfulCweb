#ifndef CWEB_HTTP_HTTPSESSION_H_
#define CWEB_HTTP_HTTPSESSION_H_

#include "tcpconnection.h"
#include "httpparser.h"
#include "http_code.h"
#include "httprequest.h"

using namespace cweb::tcpserver;

namespace cweb {
namespace httpserver {

class WebSocket;
class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
    virtual ~HttpSession();
    typedef std::function<void(std::shared_ptr<HttpSession>, std::unique_ptr<HttpRequest>)> RequestCallback;
    friend class HttpServer;
    HttpSession(std::shared_ptr<TcpConnection> conn, RequestCallback cb);
    void Init();
    
    void SendString(HttpStatusCode code, const std::string& data);
    void SendJson(HttpStatusCode code, const std::string& data);
    void SendFile(HttpStatusCode code, const std::string& filepath, std::string filename = "");
    //void SendMedia(HttpStatusCode code, const std::string& filepath, //type)
    //void SendBinary()
    //void SendHtml();
    void SendMultipart(HttpStatusCode code, const std::vector<MultipartPart*>& parts);
    void Send(ByteData* data);
    
protected:
    std::shared_ptr<TcpConnection> connection_;
    RequestCallback request_callback_;
    
private:
    friend class HttpParser;
    std::unique_ptr<HttpParser> http_parser_;
    std::shared_ptr<WebSocket> websocket_ ;
    bool need_close_ = true;
    bool upgrade_ = false;
    virtual TcpConnection::MessageState handleMessage(std::shared_ptr<TcpConnection> conn, ByteBuffer* buf, Time time);
    void handleParsedMessage(std::unique_ptr<HttpRequest> request);
    static std::string generateBoundary(size_t len);
};

}
}

#endif
