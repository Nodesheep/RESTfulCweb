#include "httpresponse.h"

namespace cweb {
namespace httpserver {

HttpResponse::HttpResponse() {}
HttpResponse::~HttpResponse() {}

void HttpResponse::SetStatusCode(HttpStatusCode code, std::string& stream) {
    static std::unordered_map<int, std::string> http_status_code = {
        {200 ,"OK"},
        {301, "Moved Permanently"},
        {400, "Bad Request"},
        {404, "Not Found"}
    };
    
    stream += "HTTP/1.1 " + std::to_string(code) + " " + http_status_code[code] + "\r\n";
}

void HttpResponse::SetHeader(const std::string &key, const std::string &value, std::string& stream) {
    stream += key + ": " + value + "\r\n";
}

void HttpResponse::SetBody(StringPiece body, std::string& stream) {
    stream += "\r\n";
    stream += body.Data();
}


}
}
