#include "cweb.h"
#include "context.h"
#include "cweb_config.h"
#include "json.h"
#include "logger.h"
#include "websocket.h"

using namespace cweb;
using namespace cweb::log;

int main() {
    //环境初始化
    EnvConfig::Init();
    
    Cweb c("127.0.0.1", 6668);
    //全局中间件
    c.Use([](std::shared_ptr<Context> c){
        LOG(LOGLEVEL_INFO, CWEB_MODULE, "cweb", "这是一个全局中间件");
        c->Next();
    });
    //普通get请求
    c.GET("/api/sayhi", [](std::shared_ptr<Context> c){
        c->STRING(StatusOK, "hi, welcome to cweb");
    });
    //表单数据请求
    c.POST("api/sayhi", [](std::shared_ptr<Context> c){
        c->STRING(StatusOK, "hi, " + c->PostForm("name") + "/" + c->PostForm("age") + "/" + c->PostForm("hobby") + ",welcome to cweb");
    });
    
    //上传multipart
    c.POST("api/multipart", [](std::shared_ptr<Context> c){
        MultipartPart* part1 = c->MultipartForm("file");
        BinaryData data = part1->BinaryValue();
        std::string filename = part1->dispositions["filename"];
        c->SaveUploadedFile(data, "../resources/", filename);
        BinaryData data1 = c->MultipartForm("name")->BinaryValue();
        c->STRING(StatusOK, "I received your data : " + std::string(data1.data, data1.size));
    });
    
    //json
    c.GET("api/info", [](std::shared_ptr<Context> c){
        Json::Value root;
        root["name"] = "lemon";
        root["sex"] = "man";
        root["age"] = 26;
        root["school"] = "xjtu";
        
        Json::Value hobby;
        hobby["sport"] = "football";
        hobby["else"] = "sing";
        
        root["hobby"] = hobby;
        
        std::stringstream body;
        Json::StreamWriterBuilder writerBuilder;
        writerBuilder["emitUTF8"] = true;
        std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
        jsonWriter->write(root, &body);
        
        c->JSON(StatusOK, body.str());
    });
    
    //带参数请求 ?key=value
    c.GET("api/echo", [](std::shared_ptr<Context> c){
        c->STRING(StatusOK, "hi, " + c->Query("name") + ",welcome to cweb");
    });
    
    //动态路由
    c.GET("/api/dynamic/:param", [](std::shared_ptr<Context> c) {
        c->STRING(StatusOK, "I got your param: " + c->Param("param"));
    });
    
    //文件下载
    c.GET("/api/download/city", [](std::shared_ptr<Context> c) {
        c->FILE(StatusOK, "../resources/city1.jpg");
    });
    
    c.GET("/api/download/bluesky", [](std::shared_ptr<Context> c) {
        c->FILE(StatusOK, "../resources/bluesky.mp4");
    });
    
    //multipart数据
    c.GET("/api/multipart/data", [](std::shared_ptr<Context> c) {
        MultipartPart* part1 = new MultipartPart();
        std::string text = "this is a text";
        part1->headers["Content-Disposition"] = "form-data; name=\"text\"";
        part1->headers["Content-Type"] = "text/plain";
        part1->SetData(text);
        
        MultipartPart* part2 = new MultipartPart();
        std::string json = "{\"name\": \"John\",\"age\": 30,\"city\": \"New York\"}";
        part2->headers["Content-Disposition"] = "form-data; name=\"json\"";
        part2->headers["Content-Type"] = "application/json";
        part2->SetData(json);
        
        MultipartPart* part3 = new MultipartPart();
        part3->headers["Content-Disposition"] = "form-data; name=\"image\"; filename=\"image.jpg\"";
        part3->headers["Content-Type"] = "image/jpeg";
        part3->SetFile("../resources/city1.jpg");
        
        c->MULTIPART(StatusOK, std::vector<MultipartPart *>{part1, part2, part3});
        delete part1;
        delete part2;
        delete part3;
    });
    
    
    //分组路由
    Group* g1 = c.Group("/group");
    //分组中间件
    g1->Use([](std::shared_ptr<Context> c){
        LOG(LOGLEVEL_INFO, CWEB_MODULE, "cweb", "这是一个分组中间件");
        c->Next();
    });
    
    g1->GET("/sayhi", [](std::shared_ptr<Context> c){
        c->STRING(StatusOK, "hi, welcome to cweb group");
    });
    
    
    //websocket
    c.GET("/ws/echo", [](std::shared_ptr<Context> c){
        std::shared_ptr<WebSocket> ws = c->Upgrade();
        
#ifdef COROUTINE
        ByteBuffer* buf = new ByteBuffer();
        while(true) {
            ssize_t n = ws->Recv(buf);
            if(n <= 0) {
                LOG(LOGLEVEL_INFO, CWEB_MODULE, "cocweb", "websocket关闭");
                delete buf;
                ws->Close();
                return;
            }else {
                ws->SendText(std::string(buf->Peek(), buf->ReadableBytes()));
                buf->ReadAll();
            }

        }
#else
        ws->SetCloseCallback([](std::shared_ptr<WebSocket>){
            LOG(LOGLEVEL_INFO, CWEB_MODULE, "cweb", "websocket关闭");
        });
        
        ws->SetMessageCallback([](std::shared_ptr<WebSocket> ws, const char* data, size_t size, MessageType type) {
            if(type == Text) {
                ws->SendText(StringPiece(data, size));
            }else {
                ws->SendBinary(data, size);
            }
        });
#endif
    });

    c.Run(2);
    return 0;
}
