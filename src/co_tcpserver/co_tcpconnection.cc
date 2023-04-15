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

void CoTcpConnection::Send(std::iostream *stream) {
    if(ownerloop_->isInLoopThread()) {
        sendStreamInLoop(stream);
    }else {
        ownerloop_->AddTask(std::bind(&CoTcpConnection::sendStreamInLoop, this, stream));
    }
}

void CoTcpConnection::Send(util::ByteBuffer* buf) {
    //TODO 此方法暂时废弃
    if(ownerloop_->isInLoopThread()) {
        sendBufferInLoop(buf);
    }else {
        ownerloop_->AddTask(std::bind(&CoTcpConnection::sendBufferInLoop, this, buf));
    }
}

void CoTcpConnection::Send(const util::StringPiece& data) {
    //TODO 此方法暂时废弃
    if(ownerloop_->isInLoopThread()) {
        sendInLoop(data.Data(), data.Size());
    }else {
        ownerloop_->AddTask(std::bind(&CoTcpConnection::sendInLoop, this, data.Data(), data.Size()));
    }
}

void CoTcpConnection::sendBufferInLoop(util::ByteBuffer *buffer) {
    sendInLoop(buffer->Peek(), buffer->ReadableBytes());
    buffer->ReadAll();
}

void CoTcpConnection::sendStreamInLoop(std::iostream *stream) {
    stream_queue_.push(stream);
    if(!event_->Writable()) event_->EnableWriting();
}

void CoTcpConnection::sendInLoop(const void *data, size_t len) {
    //TODO 此方法暂时废弃
    outputbuffer_->Append((const char*)data, len);
    if(!event_->Writable()) {
        event_->EnableWriting();
    }
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
        //不允许sendstream与另两种send方法混用
        if(!current_stream_ && stream_queue_.size() == 0) {
            size_t n = socket_->Write((void*)outputbuffer_->Peek(), outputbuffer_->ReadableBytes());
            if(n > 0) {
                outputbuffer_->ReadBytes(n);
                if(outputbuffer_->ReadableBytes() == 0) {
                    event_->DisableWriting();
                    return;
                }
            }
        }else {
            if(!current_stream_) {
                current_stream_ = stream_queue_.front();
            }
            
            if(current_stream_) {
                current_stream_->read((char*)outputbuffer_->Back(), outputbuffer_->WritableBytes());
                int got = (int)current_stream_->gcount();
                outputbuffer_->WriteBytes(got);
                if(current_stream_->peek() == EOF) {
                    stream_queue_.pop();
                    delete current_stream_;
                    current_stream_ = nullptr;
                }
                size_t write_size = socket_->Write((void*)outputbuffer_->Peek(), outputbuffer_->ReadableBytes());
                outputbuffer_->ReadBytes(write_size);
                if(outputbuffer_->ReadableBytes() == 0 && stream_queue_.size() == 0) {
                    event_->DisableWriting();
                    return;
                }
            }
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
