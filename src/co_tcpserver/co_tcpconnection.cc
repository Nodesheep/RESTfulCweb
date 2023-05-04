#include "co_tcpconnection.h"
#include "co_event.h"
#include "co_socket.h"
#include "co_eventloop.h"
#include "logger.h"

using namespace cweb::log;

namespace cweb {
namespace tcpserver {
namespace coroutine {

CoTcpConnection::CoTcpConnection(CoEventLoop* loop, CoSocket* socket, InetAddress* addr, const std::string& id)
: TcpConnection(loop, socket, addr, id) {}

CoTcpConnection::~CoTcpConnection() {}

void CoTcpConnection::Send(util::ByteData *data) {
    while(true) {
        data->Writev(socket_->Fd());
        if(!data->Remain()) {
            delete data;
            return;
        }
    }
}

void CoTcpConnection::handleClose() {
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
        size_t n = socket_->Read((void*)inputbuffer_->Peek(), inputbuffer_->WritableBytes());
        if(n > 0) {
            Time time = Time::Now();
            LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpconnection", "conn: %s 获取数据", id_.c_str());
            inputbuffer_->WriteBytes(n);
            if(message_callback_) {
                if(message_callback_(this, inputbuffer_, time) == PROCESS) {
                    continue;
                }
            }
        }else if(n == 0) {
            LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpconnection", "conn: %s 对端主动关闭", id_.c_str());
            handleClose();
            return;
        }else {
            LOG(LOGLEVEL_WARN, CWEB_MODULE, "cotcpconnection", "conn: %s 数据读取时出错", id_.c_str());
            handleClose();
            return;
        }
    }
}
    
}
}
}
