#ifndef CWEB_TCP_TCPSERVER_H_
#define CWEB_TCP_TCPSERVER_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <boost/uuid/uuid_generators.hpp>
#include "bytebuffer.h"
#include "tcpconnection.h"

namespace cweb {
namespace tcpserver {

class InetAddress;
class EventLoop;
class Event;
class Socket;
class Time;
class TcpConnection;
class Scheduler;
class TcpServer {
    
protected:
    std::shared_ptr<EventLoop> accept_loop_;
    std::unique_ptr<Socket> accept_socket_;
    std::unique_ptr<Event> accept_event_;
    std::unique_ptr<Scheduler> scheduler_;
    boost::uuids::random_generator random_generator_;
    
    bool running_ = false;
    std::unique_ptr<InetAddress> addr_;
    TcpConnection::ConnectedCallback connected_callback_;
    std::unordered_map<std::string, std::shared_ptr<TcpConnection>> living_connections_;
    
    void handleAccept();
    void handleConnectionClose(std::shared_ptr<TcpConnection> conn);
    void removeConnectionInLoop(std::shared_ptr<TcpConnection> conn);
    void init();
    
public:
    TcpServer(std::shared_ptr<EventLoop> loop, uint16_t port = 0, bool loopbackonly = false, bool ipv6 = false);
    TcpServer(std::shared_ptr<EventLoop> loop, const std::string& ip, uint16_t port, bool ipv6 = false);
    
    virtual ~TcpServer();
    void SetConnectedCallback(TcpConnection::ConnectedCallback cb) {connected_callback_ = std::move(cb);}
    
    virtual void Start(int threadcnt);
    virtual void Quit();
};

}
}

#endif
