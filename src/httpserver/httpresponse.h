#ifndef CWEB_HTTP_HTTPRESPONSE_H_
#define CWEB_HTTP_HTTPRESPONSE_H_

#include <string>
#include "http_code.h"
#include "bytebuffer.h"

using namespace cweb::tcpserver;

namespace cweb {
namespace httpserver {

class HttpResponse {
public:
    HttpResponse();
    ~HttpResponse();
    static void SetHeader(const std::string& key, const std::string& value, std::string& stream);
    static void SetStatusCode(HttpStatusCode code, std::string& stream);
    static void SetBody(StringPiece body, std::string& stream);
};

}
}

#endif 
