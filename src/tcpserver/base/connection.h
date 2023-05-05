#ifndef CWEB_TCP_CONNECTION_H_
#define CWEB_TCP_CONNECTION_H_

#include "bytebuffer.h"
#include "bytedata.h"
#include "inetaddress.h"
#include "timer.h"
#include <unordered_map>
#include <vector>
#include <queue>
#include <iostream>

namespace cweb {
namespace tcpserver {

class Connection {
public:
    enum MessageState {
        FINISH,
        PROCESS,
        BAD
    };
    
    typedef std::function<void(Connection*)> CloseCallback;
    typedef std::function<MessageState(Connection*, ByteBuffer*, Time)> MessageCallback;
    
protected:
    enum ConnectState {
        INIT,
        CONNECT,
        CLOSED
    };
    
    std::string id_ = nullptr;
    InetAddress* iaddr_ = nullptr;
    bool keep_alive_ = false;
    ConnectState connect_state_ = INIT;
    ByteBuffer* outputbuffer_ = nullptr;
    ByteBuffer* inputbuffer_ = nullptr;
    CloseCallback close_callback_;
    MessageCallback message_callback_;
    std::vector<CloseCallback> close_handlers_;
    std::queue<ByteData*> send_datas_;
    
    void handleRead(Time time);
    void handleWrite();
    void handleClose();
    void sendInLoop(const void* data, size_t len);
    void sendBufferInLoop(ByteBuffer* buffer);
    void connectEstablished();
    
public:
    Connection(InetAddress* addr, const std::string& id) : iaddr_(addr), id_(id),
    outputbuffer_(new ByteBuffer()), inputbuffer_(new ByteBuffer()) {}
    
    virtual ~Connection() {
        delete iaddr_;
        delete outputbuffer_;
        delete inputbuffer_;
    }
    
    std::string Id() const {return id_;}
    bool KeepAlive() const {return keep_alive_;}
    void SetCloseCallback(CloseCallback cb) {close_callback_ = std::move(cb);}
    //void SetRemoveRequestCallback(CloseCallback cb) {remove_request_callback_ = std::move(cb);}
    void AddCloseHandlers(CloseCallback handler) {close_handlers_.push_back(std::move(handler));}
    /* //废弃
    virtual void Send(util::ByteBuffer* buf) = 0;
    virtual void Send(const util::StringPiece& data) = 0;
    virtual void Send(std::iostream* stream) = 0;
    virtual void Send(const std::vector<std::iostream*>& streams) = 0;
     */
     
    virtual void Send(ByteData* data) = 0;
};

}
}

#endif
