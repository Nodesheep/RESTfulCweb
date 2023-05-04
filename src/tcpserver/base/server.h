#ifndef CWEB_TCP_SERVER_H_
#define CWEB_TCP_SERVER_H_

#include "inetaddress.h"
#include "bytebuffer.h"
#include "connection.h"
#include <unordered_map>

namespace cweb {
namespace tcpserver {

class Time;
class Server {
protected:
    bool running_ = false;
    InetAddress* addr_ = nullptr;
    Connection::MessageCallback message_callback_;
    std::unordered_map<std::string, Connection*> living_connections_;
    
    void handleAccept();
    void handleConnectionClose(const Connection* conn);
    void removeConnectionInLoop(const Connection* conn);
    
public:
    Server(InetAddress* addr) : addr_(addr) {}
    virtual ~Server() {
        delete addr_;
    }
    virtual void Start(int threadcnt) = 0;
    virtual void Quit() = 0;
    void SetMessageCallback(Connection::MessageCallback cb) {message_callback_ = std::move(cb);}
};

}
}

#endif
