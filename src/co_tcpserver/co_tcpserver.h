#ifndef CWEB_COROUTINE_COTCPSERVER_H_
#define CWEB_COROUTINE_COTCPSERVER_H_

#include "event.h"
#include "tcpserver.h"
#include "inetaddress.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

class CoSocket;
class CoEventLoop;
class CoEvent;
class CoScheduler;
class CoTcpServer : public TcpServer {
    
protected:
    void handleAccept();
    void handleConnectionClose(std::shared_ptr<TcpConnection> conn);
    void removeConnectionInLoop(std::shared_ptr<TcpConnection> conn);
    void init();
    
public:
    CoTcpServer(std::shared_ptr<EventLoop> loop, uint16_t port = 0, bool loopbackonly = false, bool ipv6 = false);
    CoTcpServer(std::shared_ptr<EventLoop> loop, const std::string& ip, uint16_t port, bool ipv6 = false);
    virtual ~CoTcpServer();
    
    virtual void Start(int threadcnt) override;
};

}
}
}

#endif
