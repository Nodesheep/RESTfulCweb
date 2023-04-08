#ifndef CWEB_TCP_CONNECTION_H_
#define CWEB_TCP_CONNECTION_H_

#include "bytebuffer.h"
#include "inetaddress.h"
#include "timer.h"
#include <unordered_map>
#include <vector>

namespace cweb {
namespace tcpserver {

class Connection {
protected:
    enum ConnectState {
        INIT,
        CONNECT,
        CLOSED
    };
    
    typedef std::function<void(Connection*)> CloseCallback;
    typedef std::function<void(Connection*, util::ByteBuffer*, Time)> MessageCallback;
    
    std::string id_ = nullptr;
    InetAddress* iaddr_ = nullptr;
    bool keep_alive_ = false;
    ConnectState connect_state_ = INIT;
    util::ByteBuffer* outputbuffer_ = nullptr;
    util::ByteBuffer* inputbuffer_ = nullptr;
    CloseCallback close_callback_;
    MessageCallback message_callback_;
    std::vector<CloseCallback> close_handlers_;
    
    void handleRead(Time time);
    void handleWrite();
    void handleClose();
    void sendInLoop(const void* data, size_t len);
    void sendBufferInLoop(util::ByteBuffer* buffer);
    void connectEstablished();
    
public:
    Connection(InetAddress* addr, const std::string& id) : iaddr_(addr), id_(id),
    outputbuffer_(new util::ByteBuffer()), inputbuffer_(new util::ByteBuffer()) {}
    
    virtual ~Connection() {
        delete iaddr_;
        delete outputbuffer_;
        delete inputbuffer_;
    }
    
    bool KeepAlive() const {return keep_alive_;}
    void SetCloseCallback(CloseCallback cb) {close_callback_ = std::move(cb);}
    //void SetRemoveRequestCallback(CloseCallback cb) {remove_request_callback_ = std::move(cb);}
    void AddCloseHandlers(CloseCallback handler) {close_handlers_.push_back(std::move(handler));}
    virtual void Send(util::ByteBuffer* buf) = 0;
    virtual void Send(const util::StringPiece& data) = 0;
};

}
}

#endif
