#ifndef CWEB_COROUTINE_COTCPCONNECTION_H_
#define CWEB_COROUTINE_COTCPCONNECTION_H_

#include "tcpconnection.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

class CoEventLoop;
class CoSocket;
class CoEvent;
class CoTcpConnection : public TcpConnection {
protected:
    void handleRead(Time time);
    void handleWrite();
    void handleClose();
    void handleTimeout();
    void sendInLoop(const void* data, size_t len);
    void sendBufferInLoop(util::ByteBuffer* buffer);
    void sendStreamInLoop(std::iostream* stream);
    void connectEstablished();
    
public:
    friend class CoTcpServer;
    CoTcpConnection(CoEventLoop* loop, CoSocket* socket, InetAddress* addr, const std::string& id);
    virtual ~CoTcpConnection();
    
    virtual void Send(util::ByteBuffer* buf) override;
    virtual void Send(const util::StringPiece& data) override;
    virtual void Send(std::iostream* stream) override;

};
    
}
}
}

#endif 
