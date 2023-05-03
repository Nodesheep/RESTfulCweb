#include "tcpconnection.h"
#include "eventloop.h"
#include "event.h"
#include "socket.h"
#include "inetaddress.h"
#include "logger.h"
#include <unistd.h>

using namespace cweb::log;

namespace cweb {
namespace tcpserver {

TcpConnection::TcpConnection(EventLoop* loop, Socket* socket, InetAddress* addr, const std::string& id)
: ownerloop_(loop),
  socket_(socket),
  Connection(addr, id) {}

TcpConnection::~TcpConnection() {
    if(connect_state_ == CONNECT) {
        handleClose();
    }
    
    delete event_;
    delete socket_;
}

void TcpConnection::handleRead(Time time) {
    LOG(LOGLEVEL_INFO, CWEB_MODULE, "tcpconnection", "conn: %s 获取数据", id_.c_str());
    if(timeout_timer_) {
        ownerloop_->RemoveTimer(timeout_timer_);
        timeout_timer_ = nullptr;
    }
    
    size_t n = socket_->Read((void*)inputbuffer_->Back(), inputbuffer_->WritableBytes());
    if(n > 0) {
        inputbuffer_->WriteBytes(n);
        if(message_callback_) {
            message_callback_(this, inputbuffer_, time);
        }
        timeout_timer_ = ownerloop_->AddTimer(50, std::bind(&TcpConnection::handleTimeout, this));
    }else if(n == 0) {
        LOG(LOGLEVEL_INFO, CWEB_MODULE, "tcpconnection", "conn: %s 对端主动关闭", id_.c_str());
        handleClose();
    }else {
        LOG(LOGLEVEL_WARN, CWEB_MODULE, "tcpconnection", "conn: %s 数据读取时出错", id_.c_str());
        handleClose();
    }
}

void TcpConnection::handleWrite() {
    if(event_ && event_->Writable()) {
        util::ByteData* data = send_datas_.front();
        data->Writev(socket_->Fd());
        if(!data->Remain()) {
            send_datas_.pop();
            delete data;
        }
        
        if(send_datas_.size() == 0) {
            event_->DisableWriting();
        }
    }
}

void TcpConnection::handleClose() {
    connect_state_ = CLOSED;
    for(CloseCallback handler : close_handlers_) {
        handler(this);
    }
    
    if(timeout_timer_) {
        ownerloop_->RemoveTimer(timeout_timer_);
        timeout_timer_ = nullptr;
    }
    
    event_->DisableAll();
    event_->Remove();
    close_callback_(this);
}

void TcpConnection::handleTimeout() {
    LOG(LOGLEVEL_INFO, CWEB_MODULE, "tcpconnection", "连接超时，socketfd: %d, id: %s", socket_->Fd() ,id_.c_str());
    handleClose();
}

void TcpConnection::Send(util::ByteData *data) {
    if(ownerloop_->isInLoopThread()) {
        sendInLoop(data);
    }else {
        ownerloop_->AddTask(std::bind(&TcpConnection::sendInLoop, this, data));
    }
}

void TcpConnection::sendInLoop(util::ByteData *data) {
    if(send_datas_.size() == 0 && !event_->Writable()) {
        data->Writev(socket_->Fd());
    }
    
    if(data->Remain()) {
        data->CopyDataIfNeed();
        send_datas_.push(data);
        if(!event_->Writable()) {
            event_->EnableWriting();
        }
    }else {
        delete data;
    }
}

void TcpConnection::connectEstablished() {
    event_ = new Event(ownerloop_, socket_->Fd());
    event_->SetReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    event_->SetWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    event_->EnableReading();
    timeout_timer_ = ownerloop_->AddTimer(50, std::bind(&TcpConnection::handleTimeout, this));
    connect_state_ = CONNECT;
    LOG(LOGLEVEL_INFO, CWEB_MODULE, "tcpconnection", "conn: %s established", id_.c_str());
}

/*
void TcpConnection::sendInLoop1(const void *data, size_t len) {
     size_t remain = len;
     size_t write_size = 0;
     if(outputbuffer_->ReadableBytes() == 0 && !event_->Writable()) {
         write_size = socket_->Write(data, len);
         remain -= write_size;
     }
     
     if(remain) {
         outputbuffer_->Append(static_cast<const char*>(data) + write_size, remain);
         if(!event_->Writable()) {
             event_->EnableWriting();
         }
     }
 }

void TcpConnection::Send(std::iostream *stream) {
    if(ownerloop_->isInLoopThread()) {
        sendStreamInLoop(stream);
    }else {
        ownerloop_->AddTask(std::bind(&TcpConnection::sendStreamInLoop, this, stream));
    }
}

void TcpConnection::Send(const std::vector<std::iostream *> &streams) {
    if(ownerloop_->isInLoopThread()) {
        sendStreamsInLoop(streams);
    }else {
        ownerloop_->AddTask(std::bind(&TcpConnection::sendStreamsInLoop, this, streams));
    }
}

void TcpConnection::Send(util::ByteBuffer *buf) {
    if(ownerloop_->isInLoopThread()) {
        sendBufferInLoop(buf);
    }else {
        ownerloop_->AddTask(std::bind(&TcpConnection::sendBufferInLoop, this, buf));
    }
}

void TcpConnection::Send(const util::StringPiece &data) {
    if(ownerloop_->isInLoopThread()) {
        sendInLoop1(data.Data(), data.Size());
    }else {
        ownerloop_->AddTask(std::bind(&TcpConnection::sendInLoop1, this, data.Data(), data.Size()));
    }
}

void TcpConnection::sendStreamInLoop(std::iostream *stream) {
    stream_queue_.push(stream);
    if(!event_->Writable()) event_->EnableWriting();
}

void TcpConnection::sendStreamsInLoop(const std::vector<std::iostream *> &streams) {
    for(int i = 0; i < streams.size(); ++i) {
        stream_queue_.push(streams[i]);
    }
    if(!event_->Writable()) event_->EnableWriting();
}

void TcpConnection::sendBufferInLoop(util::ByteBuffer *buffer) {
    sendInLoop1(buffer->Peek(), buffer->ReadableBytes());
    buffer->ReadAll();
}*/

/*
void TcpConnection::handleWrite() {
    //不允许sendstream与另两种send方法混用
    if(event_ && event_->Writable()) {
        //LOG(LOGLEVEL_WARN, CWEB_MODULE, "tcpconnection", "conn: %s 响应数据", id_.c_str());
        if(!current_stream_ && stream_queue_.size() == 0) {
            size_t n = socket_->Write((void*)outputbuffer_->Peek(), outputbuffer_->ReadableBytes());
            if(n > 0) {
                outputbuffer_->ReadBytes(n);
                if(outputbuffer_->ReadableBytes() == 0) {
                    event_->DisableWriting();
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
                }
            }
        }
    }
}*/

}
}

