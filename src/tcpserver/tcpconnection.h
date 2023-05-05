#ifndef CWEB_TCP_TCPCONNECTION_H_
#define CWEB_TCP_TCPCONNECTION_H_

#include "timer.h"
#include "connection.h"
#include <memory>
#include <string>
#include <functional>
#include "connection.h"

namespace cweb {
namespace tcpserver {

class EventLoop;
class Socket;
class Event;
class InetAddress;
class Time;
class Timer;
class TcpConnection : public Connection {
protected:
    void handleRead(Time time);
    void handleWrite();
    void handleClose();
    void handleTimeout();
    void sendInLoop(ByteData* data);
    void connectEstablished();
    
    EventLoop* ownerloop_ = nullptr;
    Socket* socket_ = nullptr;
    Event* event_ = nullptr;
    Timer* timeout_timer_;
    
public:
    friend class TcpServer;
    
    TcpConnection(EventLoop* loop, Socket* socket, InetAddress* addr, const std::string& id);
    virtual ~TcpConnection();
    
    bool KeepAlive() const {return keep_alive_;}
    EventLoop* Ownerloop() const {return ownerloop_;}
    
    /*
    virtual void Send(util::ByteBuffer* buf) override;
    virtual void Send(const util::StringPiece& data) override;
    virtual void Send(std::iostream* stream) override;
    virtual void Send(const std::vector<std::iostream*>& streams) override;
     */
     
    virtual void Send(ByteData* data) override;
    
};

}
}

#endif
