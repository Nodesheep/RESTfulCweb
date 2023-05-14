#ifndef CWEB_HTTP_HTTPPARSER_H_
#define CWEB_HTTP_HTTPPARSER_H_

#include "http_parser.h"
#include <memory>

namespace cweb {
namespace httpserver {

class HttpSession;
class HttpRequest;
class HttpParser {
private:
    enum ParserProcess {
        PROCESS,
        SUCCESS,
        FAIL
    };
    
    std::weak_ptr<HttpSession> session_;
    std::unique_ptr<http_parser> parser_;
    std::unique_ptr<http_parser_settings> parser_settings_;
    std::unique_ptr<HttpRequest> request_;
    ParserProcess parser_process_ = PROCESS;
    //wsparser

public:
    HttpParser(std::weak_ptr<HttpSession> session);
    ParserProcess Parse(const void* data, size_t len);
    bool CheckVersion(int major, int minor) const;
    bool IsUpgrade() const;
    
private:
    static int handleMessageBegin(http_parser* parser);
    static int handleURL(http_parser* parser, const char *at, size_t length);
    static int handleHeaderField(http_parser* parser, const char *at, size_t length);
    static int handleHeaderValue(http_parser* parser, const char *at, size_t length);
    static int handleBody(http_parser* parser, const char *at, size_t length);
    static int handleMessageComplete(http_parser* parser);
    
};

}
}

#endif

