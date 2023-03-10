#ifndef CWEB_TCP_TCPCONNECTION_H_
#define CWEB_TCP_TCPCONNECTION_H_

#include "../util/bytebuffer.h"
#include "timer.h"
#include <memory>
#include <string>
#include <functional>

namespace cweb {
namespace tcpserver {

class EventLoop;
class Socket;
class Event;
class InetAddress;
class Time;
class TcpConnection {
    
private:
    typedef std::function<void(TcpConnection*)> CloseCallback;
    typedef std::function<void(TcpConnection*, util::ByteBuffer*, Time)> MessageCallback;
    
    EventLoop* ownerloop_;
    std::string id_;
    Socket* socket_;
    Event* event_;
    InetAddress* iaddr_;
    bool keep_alive_;
    util::ByteBuffer outputbuffer_;
    util::ByteBuffer inputbuffer_;
    CloseCallback close_callback_;
    CloseCallback remove_request_callback_;
    MessageCallback message_callback_;
    
    void handleRead(Time time);
    void handleWrite();
    void handleClose();
    void sendInLoop(const void* data, size_t len);
    void sendBufferInLoop(util::ByteBuffer* buffer);
    
public:
    friend class TcpServer;
    
    TcpConnection(EventLoop* loop, Socket* socket, InetAddress* addr, const std::string& id) {};
    
    bool KeepAlive() const {return keep_alive_;}
    void Send(util::ByteBuffer* buf);
    void Send(const util::StringPiece& data);
    
    void SetCloseCallback(CloseCallback cb) {close_callback_ = std::move(cb);}
    void SetRemoveRequestCallback(CloseCallback cb) {remove_request_callback_ = std::move(cb);}
    
    EventLoop* Ownerloop() const {return ownerloop_;}
    
};

}
}

#endif
