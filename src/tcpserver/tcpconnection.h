#ifndef CWEB_TCP_TCPCONNECTION_H_
#define CWEB_TCP_TCPCONNECTION_H_

#include "bytebuffer.h"
#include "bytedata.h"
#include "inetaddress.h"
#include "timer.h"
#include <unordered_map>
#include <vector>
#include <queue>
#include <memory>
#include <string>
#include <functional>

namespace cweb {
namespace tcpserver {

class EventLoop;
class Socket;
class Event;
class InetAddress;
class Time;
class Timer;
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    friend class TcpServer;
    enum MessageState {
        PROCESS,
        FINISH,
        BAD
    };
    
    typedef std::function<void(std::shared_ptr<TcpConnection>)> ConnectedCallback;
    typedef std::function<void(std::shared_ptr<TcpConnection>)> CloseCallback;
    typedef std::function<MessageState(std::shared_ptr<TcpConnection>, ByteBuffer*, Time)> MessageCallback;
    
protected:
    enum ConnectState {
        INIT,
        CONNECT,
        CLOSED
    };
    
    std::string id_ = "";
    std::unique_ptr<InetAddress> iaddr_;
    bool keep_alive_ = false;
    ConnectState connect_state_ = INIT;
    std::unique_ptr<ByteBuffer> outputbuffer_;
    std::unique_ptr<ByteBuffer> inputbuffer_;
    CloseCallback close_callback_;
    MessageCallback message_callback_;
    ConnectedCallback connected_callback_;
    std::queue<ByteData*> send_datas_;
    std::shared_ptr<EventLoop> ownerloop_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Event> event_;
    Timer* timeout_timer_;

    void handleRead(Time time);
    void handleWrite();
    void handleClose();
    void handleTimeout();
    void sendInLoop(ByteData* data);
    void connectEstablished();
    void forceCloseInLoop();
    void cancelTimer();
    void resumeTimer();
    
public:
    
    std::string Id() const {return id_;}
    bool KeepAlive() const {return keep_alive_;}
    bool Connected() const {return connect_state_ == CONNECT;}
    void SetConnectedCallback(ConnectedCallback cb) {connected_callback_ = std::move(cb);}
    void SetCloseCallback(CloseCallback cb) {close_callback_ = std::move(cb);}
    void SetMessageCallback(MessageCallback cb) {message_callback_ = std::move(cb);}
    
    TcpConnection(std::shared_ptr<EventLoop> loop, Socket* socket, InetAddress* addr, const std::string& id);
    virtual ~TcpConnection();

    EventLoop* Ownerloop() const {return ownerloop_.get();}
    
    /*
    virtual void Send(util::ByteBuffer* buf) override;
    virtual void Send(const util::StringPiece& data) override;
    virtual void Send(std::iostream* stream) override;
    virtual void Send(const std::vector<std::iostream*>& streams) override;
     */
    virtual void ForceClose();
    virtual void Send(const void* data, size_t size);
    virtual void Send(ByteData* data);
    virtual ssize_t Recv(ByteBuffer* buf);
    
};

}
}

#endif
