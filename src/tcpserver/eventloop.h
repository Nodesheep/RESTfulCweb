#ifndef CWEB_TCP_EVENTLOOP_H_
#define CWEB_TCP_EVENTLOOP_H_

#include "../util/runloop.h"

#include <mutex>
#include <vector>
#include <thread>
#include <memory>

namespace cweb {
namespace tcpserver {

class Poller;
class Event;
class Timer;
class TimerManager;
class EventLoop : public util::RunLoop {

public:
    typedef std::function<void()> Functor;
    
    EventLoop();
    ~EventLoop();
    
    virtual void Run() override;
    virtual void Stop() override;
    virtual void Quit() override;
    
    void AddTask();
    void AddTimer(uint64_t ms, Functor cb, int repeats = 1);
    void UpdateEvent(Event* event);
    void RemoveEvent(Event* event);
    

protected:
    virtual void loop() override;
    virtual void wakeup() override;
    
private:
    Poller* poller_;
    std::mutex mutex_;
    std::vector<Functor> tasks_;
    TimerManager* timermanger_;
   // std::vector<Timer*> timers_;
    
    int wakeup_fd_[2];
    Event* wakeup_event_;
    const std::thread::id thread_id_;
    Event* active_events_;
    
};

}
}

#endif
