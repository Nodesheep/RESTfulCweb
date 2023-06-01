#ifndef CWEB_HTTP_HTTPREQUEST_H_
#define CWEB_HTTP_HTTPREQUEST_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>
#include "bytebuffer.h"
#include "json.h"
#include "multipartparser.h"

using namespace cweb::tcpserver;

namespace cweb {
namespace httpserver {

class HttpRequestBody {
protected:
    const std::string content_type_ = "";
    
public:
    virtual ~HttpRequestBody() {}
    friend class HttpRequest;
    HttpRequestBody(const std::string& type) : content_type_(type) {}
    virtual bool SetData(const char* data, size_t size) = 0;
    static HttpRequestBody* HttpRequestBodyFactory(const std::string& content_type);
};

class HttpRequestFormBody : public HttpRequestBody {
private:
    std::unordered_map<std::string, std::string> formdatas_;
    
public:
    virtual ~HttpRequestFormBody() {}
    HttpRequestFormBody() : HttpRequestBody("application/x-www-form-urlencoded") {}
    virtual bool SetData(const char* data, size_t size) override;
    const std::string& FormData(const std::string& key) const;
};

class HttpRequestJsonBody : public HttpRequestBody {
private:
    Json::Value json_;
    
public:
    HttpRequestJsonBody() : HttpRequestBody("application/json") {}
    virtual ~HttpRequestJsonBody() {}
    virtual bool SetData(const char* data, size_t size) override;
    const Json::Value& JsonValue() const;
};

struct BinaryData {
    const char* data = nullptr;
    size_t size = 0;
    BinaryData(const char* data = nullptr, size_t size = 0) : data(data), size(size) {}
};

struct MultipartPart {
    
private:
    int fd_ = -1;
    std::string content_type_ = "";
    std::string filepath_ = "";
    std::string headerstr_ = "";
    std::shared_ptr<HttpRequestBody> body_;
    std::unique_ptr<BinaryData> data_;
    size_t size_ = 0;

public:
    std::unordered_map<std::string, std::string> headers;
    std::unordered_map<std::string, std::string> dispositions;
   // const char* data = nullptr;
    //c++17 std::optional
    //std::string filepath = "";
    //size_t size = 0;
    MultipartPart();
    
    void SetData(const StringPiece& data);
    void SetData(const void* data, size_t size);
    void SetFile(const std::string& filepath);
    void SetFile(int fd, size_t size);
    
    bool ParseBody();
    
    const std::string& HeaderStr();
    int Fd() const {return fd_;}
    const char* Data() {return data_->data;}
    const size_t Size() {return size_;}
    const Json::Value& JsonValue() const;
    const BinaryData& BinaryValue() const;
    const std::string& PostForm(const std::string &key) const;
};

class HttpRequestMultipartBody : public HttpRequestBody {
private:
    std::string boundary_ = "";
    multipartparser parser_;
    multipartparser_callbacks callbacks_;
    //std::unordered_map<std::string, MultipartPart> multiparts_;
    std::vector<MultipartPart*> multiparts_;
public:
    HttpRequestMultipartBody(const std::string& content_type);
    virtual ~HttpRequestMultipartBody();
    virtual bool SetData(const char* data, size_t size) override;
    MultipartPart* MultipartForm(const std::string& key) const; //header data
    
    static int handlePartBegin(multipartparser* parser);
    static int handleHeaderField(multipartparser* parser, const char* data, size_t size);
    static int handleHeaderValue(multipartparser* parser, const char* data, size_t size);
    static int handleData(multipartparser* parser, const char* data, size_t size);
    static int handlePartEnd(multipartparser* parser);
};

class HttpRequest {
private:
    std::string method_;
    std::unordered_map<std::string, std::string> headers_;
    std::string url_ = "";
    std::string path_ = "";
    std::string content_type_ = "";
    std::unique_ptr<BinaryData> data_;
    std::unordered_map<std::string, std::string> querys_;
    std::unique_ptr<ByteBuffer> raw_body_;
    std::shared_ptr<HttpRequestBody> body_;
    
public:
    friend class HttpParser;
    friend class HttpServer;
    friend class HttpSession;
    friend class WebSocket;
    HttpRequest();
    ~HttpRequest() {};
    
    bool ParseBody();
    
    const std::string& Url() const {return url_;}
    const std::string& Method() const {return method_;}
    const std::string& Path() const {return path_;}
    const std::string& Query(const std::string& key) const {
        static std::string empty;
        auto iter = querys_.find(key);
        if(iter != querys_.end()) {
            return iter->second;
        }
        return empty;
    }
    
    //const
    const std::string& PostForm(const std::string &key) const;
    MultipartPart* MultipartForm(const std::string& key) const;
    const Json::Value& JsonValue() const;
    const BinaryData& BinaryValue() const;
    
    void HandleClose();
};

}
}

#endif

