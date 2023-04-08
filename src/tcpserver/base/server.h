#ifndef CWEB_TCP_SERVER_H_
#define CWEB_TCP_SERVER_H_

#include "inetaddress.h"
#include "bytebuffer.h"
#include <unordered_map>

namespace cweb {
namespace tcpserver {

class Connection;
class Time;
class Server {
protected:
    bool running_ = false;
    typedef std::function<void(Connection*, util::ByteBuffer*, Time)> MessageCallback;
    InetAddress* addr_ = nullptr;
    MessageCallback message_callback_;
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
    void SetMessageCallback(MessageCallback cb) {message_callback_ = std::move(cb);}
};

}
}

#endif
