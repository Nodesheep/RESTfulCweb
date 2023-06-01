#include "httpparser.h"
#include "httpsession.h"
#include "httprequest.h"
#include <algorithm>

namespace cweb {
namespace httpserver {

static std::string header_key_temp_ = "";

static std::unordered_map<unsigned int, std::string> Methods = {
    {0, "DELETE"},
    {1, "GET"},
    {2, "HEAD"},
    {3, "POST"},
    {4, "PUT"}
};

HttpParser::HttpParser(std::weak_ptr<HttpSession> session) : session_(session) {
    parser_.reset(new http_parser());
    parser_settings_.reset(new http_parser_settings());
    http_parser_init(parser_.get(), HTTP_REQUEST);
    request_.reset(new HttpRequest());
    parser_->data = this;
    http_parser_settings_init(parser_settings_.get());
    parser_settings_->on_message_begin = handleMessageBegin;
    parser_settings_->on_url = handleURL;
    parser_settings_->on_header_field = handleHeaderField;
    parser_settings_->on_header_value = handleHeaderValue;
    parser_settings_->on_body = handleBody;
    parser_settings_->on_message_complete = handleMessageComplete;
}

HttpParser::ParserProcess HttpParser::Parse(const void *data, size_t len) {
    size_t n = http_parser_execute(parser_.get(), parser_settings_.get(), (const char*)data, len);
    if(n != len) {
        parser_process_ = FAIL;
    }
    return parser_process_;
}

bool HttpParser::CheckVersion(int major, int minor) const {
    return parser_->http_major == major && parser_->http_minor == minor;
}

bool HttpParser::IsUpgrade() const {
    return parser_->upgrade;
}


int HttpParser::handleMessageBegin(http_parser* parser) {
    ((HttpParser*)(parser->data))->parser_process_ = PROCESS;
    ((HttpParser*)(parser->data))->request_.reset(new HttpRequest());
    return 0;
}

int HttpParser::handleURL(http_parser* parser, const char *at, size_t length) {
    HttpParser* self = (HttpParser*)(parser->data);
    self->request_->method_ = Methods[parser->method];
    self->request_->url_.assign(at, length);
    const char* start = at;
    const char* end = at + length;
    const char* flag = std::find(start, end, '?');
    if(start != flag) {
        self->request_->path_.assign(start, flag);
    }
    
    const char* equal = nullptr;
    start = flag + 1;
    
    if(start >= end) return 0;
    
    do {
        flag = std::find(start, end, '&');
        equal = std::find(start, flag, '=');
        if(flag == end && equal == flag) {
            self->parser_process_ = FAIL;
            return 0;
        }
        self->request_->querys_[std::string(start, equal)] = std::string(equal+1, flag);
        start = flag + 1;
    }while(flag != end);
    return 0;
}

int HttpParser::handleHeaderField(http_parser* parser, const char *at, size_t length) {
    header_key_temp_.assign(at, length);
    return 0;
}

int HttpParser::handleHeaderValue(http_parser* parser, const char *at, size_t length) {
    HttpParser* self = (HttpParser*)(parser->data);
    self->request_->headers_[header_key_temp_] = std::string(at, length);
    header_key_temp_.clear();
    return 0;
}

int HttpParser::handleBody(http_parser* parser, const char *at, size_t length) {
    HttpParser* self = (HttpParser*)(parser->data);
    //TODO 文件上传场景 缓存到磁盘中 multipart场景
    //copy
    self->request_->raw_body_->Append(at, length);
    return 0;
}

int HttpParser::handleMessageComplete(http_parser* parser) {
    HttpParser* self = (HttpParser*)(parser->data);
    self->parser_process_ = SUCCESS;
    if(auto session = self->session_.lock()) {
        session->handleParsedMessage(std::move(self->request_));
    }
    return 0;
}


}
}
