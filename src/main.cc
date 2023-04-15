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
        c->JSON(StatusOK, root);
    });
    //带参数请求 ?key=value
    c.GET("api/echo", [](Context* c){
        c->STRING(StatusOK, "hi, " + c->Query("name") + ",welcome to cweb");
    });
    //动态路由
    c.GET("/api/dynamic/:param", [](Context* c) {
        c->STRING(StatusOK, "I got your param: " + c->Param("param"));
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
