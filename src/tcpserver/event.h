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
    typedef std::function<void()> EventCallback;
    typedef std::function<void(Time)> ReadEventCallback;
    
    void SetRevents(short revents) {revents_ = revents;}
    void AddRevents(short revents) {revents_ |= revents;}
    
    void EnableReading() {events_ |= READ_EVENT;}
    void EnableWriting() {events_ |= WRITE_EVENT;}
    
private:
    int fd_ = 0;
    int events_ = 0;
    int revents_ = 0;
    int index_ = -1;
    EventLoop* loop_ = nullptr;
    ReadEventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _finishCallback;
    EventCallback _errorCallback;
};

}
}

#endif
