#include "co_tcpconnection.h"
#include "co_event.h"
#include "co_eventloop.h"
#include "socket.h"
#include "logger.h"

using namespace cweb::log;

namespace cweb {
namespace tcpserver {
namespace coroutine {

CoTcpConnection::CoTcpConnection(std::shared_ptr<CoEventLoop> loop, Socket* socket, InetAddress* addr, const std::string& id)
: TcpConnection(loop, socket, addr, id) {}

CoTcpConnection::~CoTcpConnection() {}

void CoTcpConnection::Send(ByteData *data) {
    while(true) {
        ssize_t n = data->Writev(socket_->Fd());
        if(n < 0 || !data->Remain()) {
            delete data;
            break;
        }
    }
}

void CoTcpConnection::Send(const void *data, size_t size) {
    ByteData* bdata = new ByteData();
    while(true) {
        ssize_t n = bdata->Writev(socket_->Fd());
        if(n < 0 || !bdata->Remain()) {
            delete bdata;
            break;
        }
    }
}

ssize_t CoTcpConnection::Recv(ByteBuffer* buf) {
    return buf->Readv(socket_->Fd());
}

void CoTcpConnection::ForceClose() {
    if(connect_state_ != CLOSED) {
        if(ownerloop_->isInLoopThread()) {
            forceCloseInLoop();
        }else {
            ownerloop_->AddTask(std::bind(&CoTcpConnection::forceCloseInLoop, this));
        }
    }
}

void CoTcpConnection::forceCloseInLoop() {
    if(connect_state_ != CLOSED) {
        handleClose();
    }
}

void CoTcpConnection::handleClose() {
    LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpconnection", "连接关闭，id: %s", id_.c_str());
    connect_state_ = CLOSED;
    ((CoEvent*)event_.get())->TriggerEvent();
    event_->DisableAll();
    event_->Remove();
    connected_callback_(shared_from_this());
    close_callback_(shared_from_this());
}

void CoTcpConnection::handleTimeout() {
    LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpconnection", "连接超时，socketfd: %d, id: %s", socket_->Fd() ,id_.c_str());
    handleClose();
}

void CoTcpConnection::handleMessage() {
    event_.reset(new CoEvent(std::dynamic_pointer_cast<CoEventLoop>(ownerloop_), socket_->Fd(), true));
    event_->SetTimeoutCallback(std::bind(&CoTcpConnection::handleTimeout, this));
    ownerloop_->UpdateEvent(event_.get());
    connect_state_ = CONNECT;
    connected_callback_(shared_from_this());
    while(Connected()) {
        ssize_t n = inputbuffer_->Readv(socket_->Fd());
        if(n > 0) {
            Time time = Time::Now();
            LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpconnection", "conn: %s 获取数据", id_.c_str());
            //sleep(3);
            LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpconnection", "conn: %s 睡醒", id_.c_str());
            if(message_callback_) {
                MessageState state = message_callback_(shared_from_this(), inputbuffer_.get(), time);
                if(state == BAD) {
                    break;
                }
            }
        }else if(n == 0) {
            LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpconnection", "conn: %s 对端主动关闭", id_.c_str());
            handleClose();
            break;
        }else {
            LOG(LOGLEVEL_WARN, CWEB_MODULE, "cotcpconnection", "conn: %s 数据读取时出错", id_.c_str());
            handleClose();
            break;
        }
    }
}
    
}
}
}
