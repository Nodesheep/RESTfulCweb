#ifndef CWEB_TCP_EVENTLOOP_H_
#define CWEB_TCP_EVENTLOOP_H_

#include <mutex>
#include <vector>
#include <thread>
#include <memory>

namespace cweb {
namespace tcpserver {

class Poller;
class Event;
class Timer;
class Time;
class TimerManager;
class EventLoop {

public:
    typedef std::function<void()> Functor;
    
    EventLoop();
    ~EventLoop();
    
    virtual void Run();
    virtual void Stop();
    virtual void Quit();
    
    virtual void AddTask(Functor cb);
    virtual void AddTasks(std::vector<Functor>& cbs);
    virtual Timer* AddTimer(uint64_t ms, Functor cb, int repeats = 1);
    virtual void UpdateEvent(Event* event);
    virtual void RemoveEvent(Event* event);
    
    bool isInLoopThread() const {return thread_id_ == std::this_thread::get_id();}

protected:
    bool running_ = false;
    std::mutex mutex_;
    TimerManager* timermanager_;
    
    virtual void loop();
    virtual void wakeup();
    
    void createWakeupfd();
    void handleActiveEvents(Time time);
    void handleTasks();
    void handleTimeoutTimers();
    void handleWakeup();
    
private:
    Poller* poller_;
    std::vector<Functor> tasks_;
    std::vector<Timer*> timeout_timers_;
   // std::vector<Timer*> timers_;
    
    int wakeup_fd_[2];
    Event* wakeup_event_;
    const std::thread::id thread_id_;
    std::vector<Event*> active_events_;
    
};

}
}

#endif
