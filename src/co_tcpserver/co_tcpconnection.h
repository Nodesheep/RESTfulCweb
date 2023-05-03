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
    void connectEstablished();
    
public:
    friend class CoTcpServer;
    CoTcpConnection(CoEventLoop* loop, CoSocket* socket, InetAddress* addr, const std::string& id);
    virtual ~CoTcpConnection();

    virtual void Send(util::ByteData* data) override;
};
    
}
}
}

#endif 
