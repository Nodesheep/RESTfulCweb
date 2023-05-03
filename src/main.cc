#include "cweb.h"
#include "context.h"
#include "cweb_config.h"
#include "json.h"
#include "logger.h"

using namespace cweb;
using namespace cweb::log;

int main() {
    //环境初始化
    EnvConfig::Init();
    
    Cweb c("127.0.0.1", 6666);
    //全局中间件
    c.Use([](Context* c){
        LOG(LOGLEVEL_INFO, CWEB_MODULE, "cweb", "这是一个全局中间件");
        c->Next();
    });
    //普通get请求
    c.GET("/api/sayhi", [](Context* c){
        c->STRING(StatusOK, "hi, welcome to cweb");
    });
    //表单数据请求
    
    c.POST("api/sayhi", [](Context* c){
        c->STRING(StatusOK, "hi, " + c->PostForm("name") + ",welcome to cweb");
    });
    //json
    c.GET("api/info", [](Context* c){
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
    c.GET("api/echo", [](Context* c){
        c->STRING(StatusOK, "hi, " + c->Query("name") + ",welcome to cweb");
    });
    //动态路由
    c.GET("/api/dynamic/:param", [](Context* c) {
        c->STRING(StatusOK, "I got your param: " + c->Param("param"));
    });
    
    //文件下载
    c.GET("/api/download/city", [](Context* c) {
        c->FILE(StatusOK, "../resources/city1.jpg");
    });
    
    c.GET("/api/download/bluesky", [](Context* c) {
        c->FILE(StatusOK, "../resources/bluesky.mp4");
    });
    
    //multipart数据
    c.GET("/api/multipart/data", [](Context* c) {
        MultipartPart* part1 = new MultipartPart();
        part1->SetHeader("Content-Disposition", "form-data; name=\"text\"");
        part1->SetData("this is a text");
        
        MultipartPart* part2 = new MultipartPart();
        part2->SetHeader("Content-Disposition", "form-data; name=\"json\"");
        part2->SetHeader("Content-Type", "application/json");
        part2->SetData("{\"name\": \"John\",\"age\": 30,\"city\": \"New York\"}");
        
        MultipartPart* part3 = new MultipartPart();
        part3->SetHeader("Content-Disposition", "form-data; name=\"image\"; filename=\"image.jpg\"");
        part3->SetHeader("Content-Type", "image/jpeg");
        part3->SetFile("../resources/city1.jpg");
        
        c->MULTIPART(StatusOK, std::vector<MultipartPart *>{part1, part2, part3});
        delete part1;
        delete part2;
        delete part3;
    });
    
    
    //分组路由
    Group* g1 = c.Group("/group");
    //分组中间件
    g1->Use([](Context* c){
        LOG(LOGLEVEL_INFO, CWEB_MODULE, "cweb", "这是一个分组中间件");
        c->Next();
    });
    
    g1->GET("/sayhi", [](Context* c){
        c->STRING(StatusOK, "hi, welcome to cweb group");
    });

    c.Run(2);
    return 0;
}
