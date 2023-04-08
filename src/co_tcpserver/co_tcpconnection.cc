#include "co_tcpconnection.h"
#include "co_event.h"
#include "co_socket.h"
#include "co_eventloop.h"
#include <iostream>

namespace cweb {
namespace tcpserver {
namespace coroutine {

CoTcpConnection::CoTcpConnection(CoEventLoop* loop, CoSocket* socket, InetAddress* addr, const std::string& id)
: TcpConnection(loop, socket, addr, id) {}

CoTcpConnection::~CoTcpConnection() {
    
}

void CoTcpConnection::handleRead(Time time) {
    while(true) {
        std::cout << "请求链路：" << "读取消息：" << socket_->Fd() << std::endl;
        size_t n = socket_->Read((void*)inputbuffer_->Peek(), inputbuffer_->WritableBytes());
        if(n > 0) {
            inputbuffer_->WriteBytes(n);
            message_callback_(this, inputbuffer_, time);
            return;
        }else if(n == 0) {
            handleClose();
            return;
        }
    }
}

void CoTcpConnection::handleWrite() {
    while(true) {
        std::cout << "请求链路：" << "发送消息：" << socket_->Fd() << std::endl;
        size_t n = socket_->Write((void*)outputbuffer_->Peek(), outputbuffer_->ReadableBytes());
        if(n > 0) {
            outputbuffer_->ReadBytes(n);
            if(outputbuffer_->ReadableBytes() == 0) {
                event_->DisableWriting();
                return;
            }
        }
    }
}

void CoTcpConnection::handleClose() {
    std::cout << "请求链路：" << "请求销毁：" << socket_->Fd() << std::endl;
    connect_state_ = CLOSED;
    for(CloseCallback handler : close_handlers_) {
        handler(this);
    }
    event_->DisableAll();
    event_->Remove();
    close_callback_(this);
}

void CoTcpConnection::connectEstablished() {
    //
    event_ = new CoEvent((CoEventLoop*)ownerloop_, socket_->Fd());
    event_->SetReadCallback(std::bind(&CoTcpConnection::handleRead, this, std::placeholders::_1));
    event_->SetWriteCallback(std::bind(&CoTcpConnection::handleWrite, this));
    event_->EnableReading();
    std::cout << "请求链路：" << "请求建立：" << socket_->Fd() << std::endl;
    ((CoEvent*)event_)->ExecuteReadCoroutine();
    //((CoEvent*)event_)->ExecuteReadCoroutine();
}
    
}
}
}
