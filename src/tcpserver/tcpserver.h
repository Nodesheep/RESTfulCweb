#ifndef CWEB_TCP_TCPSERVER_H_
#define CWEB_TCP_TCPSERVER_H_

#include <string>
#include <memory>
#include <unordered_map>

namespace cweb {
namespace tcpserver {

class InetAddress;
class EventLoop;
class Event;
class Socket;
class Time;
class TcpConnection;
class TcpServer {
private:
    EventLoop* accept_loop_;
    Socket* accept_socket_;
    Event* accept_event_;
    InetAddress* addr_;
    std::unordered_map<std::string, TcpConnection*> living_connections_;
    void init();
    void handleAccept();
    void handleConnectionClose(const TcpConnection* conn);
    void removeConnectionInLoop(const TcpConnection* conn);
    
public:
    TcpServer(EventLoop* loop, uint16_t port = 0, bool loopbackonly = false, bool ipv6 = false);
    TcpServer(EventLoop* loop, const std::string& ip, uint16_t port, bool ipv6 = false);
    
    void Start(int threadcnt);
    void Stop();
    
};

}
}

#endif
