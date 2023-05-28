#ifndef CWEB_HTTP_HTTPRESPONSE_H_
#define CWEB_HTTP_HTTPRESPONSE_H_

#include <string>
#include <unordered_map>
#include "http_code.h"
#include "bytebuffer.h"

using namespace cweb::tcpserver;

namespace cweb {
namespace httpserver {

class HttpResponse {
private:
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;
    
    
public:
    HttpResponse();
    ~HttpResponse();
    void SetHeader(const void* data, size_t size);
    void SetBody(const void* data, size_t size);
    
    
    static void SetHeader(const std::string& key, const std::string& value, std::string& stream);
    static void SetStatusCode(HttpStatusCode code, std::string& stream);
    static void SetBody(StringPiece body, std::string& stream);
};

}
}

#endif 
