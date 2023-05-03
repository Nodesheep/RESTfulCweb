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
    //TODO 重复数据拷贝 待优化
    data->CopyDataIfNeed();
    send_datas_.push(data);
    if(!event_->Writable()) event_->EnableWriting();
}

void CoTcpConnection::handleRead(Time time) {
    while(true) {
        size_t n = socket_->Read((void*)inputbuffer_->Peek(), inputbuffer_->WritableBytes());
        if(n > 0) {
            LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpconnection", "conn: %s 获取数据", id_.c_str());
            inputbuffer_->WriteBytes(n);
            message_callback_(this, inputbuffer_, time);
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

void CoTcpConnection::handleWrite() {
    while(true) {
        util::ByteData* data = send_datas_.front();
        //TODO 有问题 没有使用hook_write 
        data->Writev(socket_->Fd());
        if(!data->Remain()) {
            send_datas_.pop();
            delete data;
        }
        
        if(send_datas_.size() == 0) {
            event_->DisableWriting();
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

void CoTcpConnection::connectEstablished() {
    event_ = new CoEvent((CoEventLoop*)ownerloop_, socket_->Fd());
    event_->SetReadCallback(std::bind(&CoTcpConnection::handleRead, this, std::placeholders::_1));
    event_->SetWriteCallback(std::bind(&CoTcpConnection::handleWrite, this));
    event_->SetTimeoutCallback(std::bind(&CoTcpConnection::handleTimeout, this));
    event_->EnableReading();
    LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpconnection", "conn: %s established", id_.c_str());
}
    
}
}
}
