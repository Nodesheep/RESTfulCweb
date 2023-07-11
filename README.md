# CWEB: 一个RESTful风格的C++Web框架

![restfulcweb](https://github.com/Nodesheep/RESTfulCweb/assets/62785394/955ad8d7-7158-43d5-9435-5c465d4598a6)

- 支持协程版线程版两种模式
- 支持路由注册、路由分组
  - RESTful风格的注册接口
  - 支持动态路由
- 支持全局中间件、分组中间件
- 支持HTTP1.1部分能力
  - JSON数据，项目中使用JSONCPP能力支持
  - 表单数据
  - multipart数据
  - 文件
- 支持WebSocket协议
- 支持部分数据库操作
  - mysql
  - redis单机与集群
  - redis分布式锁

## 项目结构
项目从上往下可分为框架层(CWEB)、协议层(HTTPSERVER)、传输层(TCPSERVRE)以及基础组件层(LOG和UTils)。CWEB主要负责路由注册与匹配，HTTPSERVER负责将数据封装成HTTPRequset交由CWEB处理，TCPSERVER负责数据传输，目前提供了线程版和协程版两种模式，LOG是项目的日志组件，Utils中包含了项目中通用的基础能力。

![项目结构](https://user-images.githubusercontent.com/62785394/232182858-82dcc9c4-4c48-4da8-8179-643b50d8f4e4.png)

## 示例
### 启动运行
```
#include "cweb.h"
using namespace cweb;

Cweb c("127.0.0.1", 6668);
//开启线程数
c.Run(2);
```

### 路由注册
```
//普通get请求
c.GET("/api/sayhi", [](std::shared_ptr<Context> c){
    c->STRING(StatusOK, "hi, welcome to cweb");
});
    
//json数据
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

//表单数据请求
c.POST("api/sayhi", [](std::shared_ptr<Context> c){
    c->STRING(StatusOK, "hi, " + c->PostForm("name") + "/" + c->PostForm("age") + "/" + c->PostForm("hobby") + ",welcome to cweb");
});

//multipart数据
c.POST("api/multipart", [](std::shared_ptr<Context> c){
    MultipartPart* part1 = c->MultipartForm("file");
    BinaryData data = part1->BinaryValue();
    std::string filename = part1->dispositions["filename"];
    //文件存储
    c->SaveUploadedFile(data, "../resources/", filename);
    BinaryData data1 = c->MultipartForm("name")->BinaryValue();
    c->STRING(StatusOK, "I received your data : " + std::string(data1.data, data1.size));
});

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

//文件传输
c.GET("/api/download/city", [](std::shared_ptr<Context> c) {
    c->FILE(StatusOK, "../resources/city1.jpg");
});
```

### 分组路由
```
Group* g1 = c.Group("/group");
g1->GET("/sayhi", [](std::shared_ptr<Context> c){
    c->STRING(StatusOK, "hi, welcome to cweb group");
});
```
### 中间件
```
//全局中间件
c.Use([](std::shared_ptr<Context> c){
    LOG(LOGLEVEL_INFO, CWEB_MODULE, "cweb", "这是一个全局中间件");
    c->Next();
});

Group* g1 = c.Group("/group");
//分组中间件
g1->Use([](std::shared_ptr<Context> c){
    LOG(LOGLEVEL_INFO, CWEB_MODULE, "cweb", "这是一个分组中间件");
    c->Next();
});

g1->GET("/sayhi", [](std::shared_ptr<Context> c){
    c->STRING(StatusOK, "hi, welcome to cweb group");
});
```

### WebSocket
```
c.GET("/ws/echo", [](std::shared_ptr<Context> c){
//获取websocket通道
    std::shared_ptr<WebSocket> ws = c->Upgrade();

//协程版可用
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
//协程线程版均可用
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
```
### Redis操作
```
c.GET("/api/redis/data", [](std::shared_ptr<Context> c) {
    RedisReplyPtr r = c->Redis()->Cmd("GET key_htl_hlea_tlyyyy_ghhtl_aseghlea");
    c->STRING(StatusOK, r->str);
});
```

### Redis分布式锁
```
//lock
c.GET("api/redis/lock", [](std::shared_ptr<Context> c) {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    static std::string key = "lockkey-" + std::to_string(time.tv_nsec);
    std::string value = c->Redis()->Lock(key, 600 * 1000);
    if(value.size() > 0) {
        c->STRING(StatusOK, "lock success key:" + key + "value:" + value);
    }else {
        c->STRING(StatusOK, "lock failed key:" + key);
    }
});

//unlock
c.GET("api/redis/unlock/:key/:value", [](std::shared_ptr<Context> c) {
    LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "redis", "key: %s value: %s", c->Param("key").c_str(), c->Param("value").c_str());
    bool res = c->Redis()->Unlock(c->Param("key"), c->Param("value"));
    if(res) {
        c->STRING(StatusOK, "unlock success");
    }else {
        c->STRING(StatusOK, "unlock failed");
    }
});
```

### MySQL
```
c.GET("/api/mysql/data", [](std::shared_ptr<Context> c) {
    RedisReplyPtr rr = c->Redis()->Cmd("GET /api/mysql/data");
    std::stringstream data;
    Json::Value res;
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["emitUTF8"] = true;
    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    if(rr && rr->str) {
        res["type"] = "redis cache";
        res["data"] = rr->str;
        jsonWriter->write(res, &data);
        c->JSON(StatusOK, data.str().c_str());
        return;
    }

    MySQLReplyPtr mr = c->MySQL()->Cmd("SELECT * FROM person");
    int rows = mr->Rows();
    res["count"] = rows;
    for(int i = 0; i < rows; ++i) {
        mr->Next();
        Json::Value p;
        p["id"] = mr->IntValue(0);
        p["name"] = mr->StringValue(1);
        p["age"] = mr->IntValue(2);
        res["people"].append(p);
    }

    Json::Value res1;
    res1["type"] = "mysql";
    res1["data"] = res;

    jsonWriter->write(res1, &data);
    c->Redis()->Cmd("SET /api/mysql/data %s PX %d", data.str().c_str(), 10 * 1000);

    c->JSON(StatusOK, data.str());
});
```

## 编译运行
### 注意事项
- 项目使用CMake进行构建，编译器需支持C++11，项目测试时使用的是g++13.1.0和Apple clang 14.0.3
- 项目依赖boost、mysqlclient以及hiredis-vip（需使用项目thirdparty中的），若无需数据库相关的功能，可删除db目录下的代码
- 项目基于mac开发，linux环境下并没有进行充分验证，后续有精力将进行完善

### 编译
推荐使用
```
mkdir build
mkdir logfile //日志存储目录
cd build
//线程版 poll
cmake ..
make
./CWEBSERVER
```
你也可以通过以下命令编译不同版本
```
//协程版 poll
cmake -DBUILD_FLAG=CPOLL ..

//linux平台 线程版 epoll
cmake -DBUILD_FLAG=TEPOLL ..

//linux平台 协程版 epoll
cmake -DBUILD_FLAG=CEPOLL ..

//unix平台 线程版 kqueue
cmake -DBUILD_FLAG=TKQUEUE ..

//unix平台 协程版 kqueue
cmake -DBUILD_FLAG=CKQUEUE ..

```

## 系列文章
持续更新中...  

框架层介绍:https://zhuanlan.zhihu.com/p/626366230

协程原理:https://zhuanlan.zhihu.com/p/627824410

线程模型与协程模型:https://zhuanlan.zhihu.com/p/627383336

http1.1与weboskcet相关:https://zhuanlan.zhihu.com/p/629289536

## 联系我
由于个人精力有限，项目可能存在很多我无法感知的问题，如果你有任何好的想法和建议请联系我，期待与你交流成长。

<img width="184" alt="截屏2023-06-05 14 15 11" src="https://github.com/Nodesheep/RESTfulCweb/assets/62785394/166c35b8-6980-4bcf-97aa-c8623104dc60">




