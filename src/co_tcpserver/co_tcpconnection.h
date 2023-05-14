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
    void forceCloseInLoop();
    
public:
    friend class CoTcpServer;
    CoTcpConnection(std::shared_ptr<CoEventLoop> loop, Socket* socket, InetAddress* addr, const std::string& id);
    virtual ~CoTcpConnection();

    virtual void ForceClose() override;
    virtual void Send(const void* data, size_t size) override;
    virtual void Send(ByteData* data) override;
    virtual ssize_t Recv(ByteBuffer* buf) override;
};
    
}
}
}

#endif 
