#include "httpserver.h"
#include "eventloop.h"

namespace cweb {
namespace http {

void HttpServer::handleMessage(TcpConnection* conn, ByteBuffer* buf, Time time) {
    if(conn->KeepAlive()) {
        handleKeepAliveMessage(conn, buf, time);
    }else {
        HttpRequest* req = conn_req_[conn];
        if(req == nullptr) {
            req = new HttpRequest();
            conn_req_[conn] = req;
            //conn->SetRemoveRequestCallback(std::bind(&HttpServer::removeRequest, this, std::placeholders::_1));
        }
        
        int res = req->Parse(buf, buf->ReadableBytes());
        //解析完
        if(res == 0) {
            if(request_callback_) {
                request_callback_(conn, req);
            }
           // delete <#expression#>
        //出错
        }else if(res < 0) {
            
        //解析中
        }else {
            //在短连接中不需要单独为request设置销毁定时器，跟随connection一同消亡
            conn->SetRemoveRequestCallback(std::bind(&HttpServer::removeRequest, this, std::placeholders::_1));
        }
        //解析完的话送到框架层然后销毁
    }
}

void HttpServer::handleKeepAliveMessage(TcpConnection *conn, ByteBuffer *buf, Time time) {
    //TODO
    //分帧
    //解析头 上一针解析完才解析头
    //某个请求超时无法销毁
    //解析中给一个定时器 一直没有拿到完整数据 req中有一个timer
    conn->SetRemoveRequestCallback(std::bind(&HttpServer::removeRequest, this, std::placeholders::_1));
    HttpRequest* req = new HttpRequest();
    if(req->remove_timer_ != nullptr) {
        req->remove_timer_->Cancel();
        req->remove_timer_ = nullptr;
    }
    int res = req->Parse(buf, buf->ReadableBytes());
    if(res == 0) {
        //交由上层处理
        //remove
        delete req;
    }else if(res < 0) {
        //remove
        delete req;
    }else {
        conn->Ownerloop()->AddTimer(10000, [conn, req](){
            delete req;
        });
    }
}

void HttpServer::removeRequest(TcpConnection *conn) {
    if(conn->KeepAlive()) {
        if(conn_reqs_.find(conn) != conn_reqs_.end()) {
            std::unordered_map<const std::string, HttpRequest*> map = conn_reqs_[conn];
            for(std::unordered_map<const std::string, HttpRequest*>::iterator iter = map.begin(); iter != map.end(); ++iter) {
                delete iter->second;
            }
            conn_reqs_.erase(conn);
        }
    }else {
        HttpRequest* req = conn_req_[conn];
        if(req != nullptr) {
            conn_req_.erase(conn);
            delete req;
        }
    }
}

}
}
