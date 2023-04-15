# CWEB: 基于C++的restful风格的web框架

## 项目结构
项目从上往下可分为框架层(CWEB)、协议层(HTTPSERVER)、传输层(TCPSERVRE)以及基础组件层(LOG和UTils)。CWEB主要负责路由注册与匹配，HTTPSERVER负责将数据封装成HTTPRequset交由CWEB处理，TCPSERVER负责数据传输，目前提供了线程版和协程版两种模式，LOG是项目的日志组件，Utils中包含了项目中通用的基础能力，如无锁队列、内存池、buffer等。

![项目结构](https://user-images.githubusercontent.com/62785394/232182858-82dcc9c4-4c48-4da8-8179-643b50d8f4e4.png)

## 框架使用
项目接口遵循Restful设计风格，支持基本路由注册以及中间件注入。
```
    //初始化
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
    
    //开启服务
    c.Run(2);
```
## 编译运行
```
  项目目前支持在linux和类unix平台运行。
  //如果没有build目录请在src同级目录下创建build目录
  cd build

  //linux和unix平台均可以通过以下指令编译运行项目
  //线程版 poll
  cmake ..
  make
  ./CWEBSERVER
  //协程版 poll
  cmake -DBUILD=CPOLL ..
  make
  ./CWEBSERVER

  //linux平台
  //线程版 epoll
  cmake -DBUILD_FLAG=TEPOLL ..
  make
  ./CWEBSERVER
  //协程版 epoll
  cmake -DBUILD_FLAG=CEPOLL ..
  make
  ./CWEBSERVER

  //unix平台
  //线程版 kqueue
  cmake -DBUILD_FLAG=TKQUEUE ..
  make
  ./CWEBSERVER
  //协程版 kqueue
  cmake -DBUILD_FLAG=CKQUEUE ..
  make
  ./CWEBSERVER
```
