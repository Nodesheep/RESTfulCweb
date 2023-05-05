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
    void handleMessage();
    void handleClose();
    void handleTimeout();
    
public:
    friend class CoTcpServer;
    CoTcpConnection(CoEventLoop* loop, Socket* socket, InetAddress* addr, const std::string& id);
    virtual ~CoTcpConnection();

    virtual void Send(ByteData* data) override;
};
    
}
}
}

#endif 
