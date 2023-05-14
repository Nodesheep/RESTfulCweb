#ifndef CWEB_TCP_EVENT_H_
#define CWEB_TCP_EVENT_H_


#include <functional>
#include <fcntl.h>

namespace cweb {
namespace tcpserver {

#define NONE_EVENT 0x0000
#define READ_EVENT 0x0001
#define WRITE_EVENT 0x0004
#define HUP_EVENT 0x0010
#define ERR_EVENT 0x0008

class EventLoop;
class Time;
class KqueuePoller;

class Event {
public:
    friend KqueuePoller;
    friend class PollPoller;
    friend class EPollPoller;
    friend EventLoop;
    Event(std::shared_ptr<EventLoop> loop, int fd, bool is_socket = false) : loop_(loop), fd_(fd), is_socket_(is_socket) {
        flags_ = ::fcntl(fd_, F_GETFL, 0);
    }
    
    typedef std::function<void()> EventCallback;
    typedef std::function<void(Time)> ReadEventCallback;
    
    void SetRevents(short revents) {revents_ = revents;}
    void AddRevents(short revents) {revents_ |= revents;}
    
    void EnableReading();
    void EnableWriting();
    void DisableReading();
    void DisableWriting();
    void DisableAll();
    
    virtual void HandleEvent(Time receiveTime);
    
    bool Readable() const {return events_ & READ_EVENT;}
    bool Writable() const {return events_ & WRITE_EVENT;}

    void HandleTimeout();
    void Remove();
    
    void SetReadCallback(ReadEventCallback cb) { read_callback_ = std::move(cb); }
    void SetWriteCallback(EventCallback cb) { write_callback_ = std::move(cb); }
    void SetTimeoutCallback(EventCallback cb) { timeout_callback_ = std::move(cb); }
    void SetErrorCallback(EventCallback cb) { error_callback_ = std::move(cb); }
    
    int Fd() const {return fd_;}
    int Flags() const {return flags_;}
    bool IsSocket() const {return is_socket_;}
    
protected:
    int fd_ = 0;
    int events_ = 0;
    int revents_ = 0;
    int index_ = -1;
    int flags_ = 0;
    bool is_socket_ = false;
    std::shared_ptr<EventLoop> loop_;
    ReadEventCallback read_callback_;
    EventCallback write_callback_;
    EventCallback timeout_callback_;
    EventCallback error_callback_;
    
    void update();
};

}
}

#endif
