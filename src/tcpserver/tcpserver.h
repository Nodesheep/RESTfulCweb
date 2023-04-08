#ifndef CWEB_TCP_TCPSERVER_H_
#define CWEB_TCP_TCPSERVER_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <boost/uuid/uuid_generators.hpp>
#include "bytebuffer.h"
#include "server.h"

namespace cweb {
namespace tcpserver {

class InetAddress;
class EventLoop;
class Event;
class Socket;
class Time;
class TcpConnection;
class Scheduler;
class TcpServer : public Server {
    
protected:
    EventLoop* accept_loop_ = nullptr;
    Socket* accept_socket_ = nullptr;
    Event* accept_event_ = nullptr;
    Scheduler* scheduler_ = nullptr;
    boost::uuids::random_generator random_generator_;
    
    void handleAccept();
    void handleConnectionClose(const Connection* conn);
    void removeConnectionInLoop(const Connection* conn);
    void init();
    
public:
    TcpServer(EventLoop* loop, uint16_t port = 0, bool loopbackonly = false, bool ipv6 = false);
    TcpServer(EventLoop* loop, const std::string& ip, uint16_t port, bool ipv6 = false);
    
    virtual ~TcpServer();
    
    virtual void Start(int threadcnt) override;
    virtual void Quit() override;
    
};

}
}

#endif
