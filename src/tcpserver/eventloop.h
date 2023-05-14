#ifndef CWEB_TCP_EVENTLOOP_H_
#define CWEB_TCP_EVENTLOOP_H_

#include <mutex>
#include <vector>
#include <pthread.h>
#include <memory>
#include "threadlocal_memorypool.h"

namespace cweb {

namespace tcpserver {

class Poller;
class Event;
class Timer;
class Time;
class TimerManager;
class EventLoop : public std::enable_shared_from_this<EventLoop> {

public:
    typedef std::function<void()> Functor;
    
    EventLoop();
    ~EventLoop();
    
    virtual void Run();
    virtual void Quit();
    
    virtual void AddTask(Functor cb);
    virtual void AddTasks(std::vector<Functor>& cbs);
    virtual Timer* AddTimer(uint64_t s, Functor cb, int repeats = 1);
    void RemoveTimer(Timer* timer);
    virtual void UpdateEvent(Event* event);
    virtual void RemoveEvent(Event* event);
    
    bool isInLoopThread() const {return tid_ == pthread_self();}

protected:
    bool running_ = false;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<util::MemoryPool> memorypool_;
    std::mutex mutex_;
    std::unique_ptr<TimerManager> timermanager_;
    std::vector<Event*> active_events_;
    
    void loop();
    void wakeup();
    
    void createWakeupfd();
    void handleActiveEvents(Time time);
    void handleTasks();
    void handleTimeoutTimers();
    void handleWakeup();
    
private:
    std::vector<Functor> tasks_;
    std::vector<Timer*> timeout_timers_;
    
    int wakeup_fd_[2];
    std::unique_ptr<Event> wakeup_event_;
    pthread_t tid_;
    
};


}
}

#endif
