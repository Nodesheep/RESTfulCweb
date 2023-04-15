#ifndef CWEB_TCP_EVENT_H_
#define CWEB_TCP_EVENT_H_


#include <functional>

namespace cweb {
namespace tcpserver {

#define NONE_EVENT 0
#define READ_EVENT 1
#define WRITE_EVENT 4

class EventLoop;
class Time;
class KqueuePoller;

class Event {
public:
    friend KqueuePoller;
    friend class PollPoller;
    friend class EPollPoller;
    friend EventLoop;
    Event(EventLoop* loop, int fd) : loop_(loop), fd_(fd) {}
    
    typedef std::function<void()> EventCallback;
    typedef std::function<void(Time)> ReadEventCallback;
    
    void SetRevents(short revents) {revents_ = revents;}
    void AddRevents(short revents) {revents_ |= revents;}
    
    void EnableReading();
    void EnableWriting();
    void DisableReading();
    void DisableWriting();
    void DisableAll();
    
    bool Readable() const {return events_ & READ_EVENT;}
    bool Writable() const {return events_ & WRITE_EVENT;}
    
    virtual void HandleEvent(Time receiveTime);
    void HandleTimeout();
    
    void Remove();
    
    void SetReadCallback(ReadEventCallback cb) { read_callback_ = std::move(cb); }
    void SetWriteCallback(EventCallback cb) { write_callback_ = std::move(cb); }
    void SetTimeoutCallback(EventCallback cb) { timeout_callback_ = std::move(cb); }
    void SetErrorCallback(EventCallback cb) { error_callback_ = std::move(cb); }
    
    int Fd() const {return fd_;}
    
protected:
    int fd_ = 0;
    int events_ = 0;
    int revents_ = 0;
    int index_ = -1;
    EventLoop* loop_ = nullptr;
    ReadEventCallback read_callback_;
    EventCallback write_callback_;
    EventCallback timeout_callback_;
    EventCallback error_callback_;
    
    void update();
};

}
}

#endif
