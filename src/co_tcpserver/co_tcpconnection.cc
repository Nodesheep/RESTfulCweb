#include "co_tcpconnection.h"
#include "co_event.h"
#include "co_eventloop.h"
#include "socket.h"
#include "logger.h"

using namespace cweb::log;

namespace cweb {
namespace tcpserver {
namespace coroutine {

CoTcpConnection::CoTcpConnection(CoEventLoop* loop, Socket* socket, InetAddress* addr, const std::string& id)
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

void CoTcpConnection::handleClose() {
    LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpconnection", "连接关闭，id: %s", id_.c_str());
    connect_state_ = CLOSED;
    for(CloseCallback handler : close_handlers_) {
        handler(this);
    }
    event_->DisableAll();
    event_->Remove();
    close_callback_(this);
}

void CoTcpConnection::handleTimeout() {
    LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpconnection", "连接超时，socketfd: %d, id: %s", socket_->Fd() ,id_.c_str());
    handleClose();
}

void CoTcpConnection::handleMessage() {
    event_ = new CoEvent((CoEventLoop*)ownerloop_, socket_->Fd());
    event_->SetTimeoutCallback(std::bind(&CoTcpConnection::handleTimeout, this));
    ownerloop_->UpdateEvent(event_);
    while(true) {
        ssize_t n = socket_->Read((void*)inputbuffer_->Peek(), inputbuffer_->WritableBytes());
        if(n > 0) {
            Time time = Time::Now();
            LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpconnection", "conn: %s 获取数据", id_.c_str());
            inputbuffer_->WriteBytes(n);
            //sleep(3);
            //LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpconnection", "conn: %s 睡醒", id_.c_str());
            if(message_callback_) {
                MessageState state = message_callback_(this, inputbuffer_, time);
                if(state == BAD) {
                    handleClose();
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
