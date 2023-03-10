#include "eventloop.h"
#include "event.h"
#include "timer.h"

#ifdef UNIX
#include "kqueue_poller.h"
#elsif LINUX
#include "epoll_poller.h"
#else
#include "poll_poller.h"
#endif

#include <unistd.h>

namespace cweb {
namespace tcpserver {

EventLoop::EventLoop()
:thread_id_(std::this_thread::get_id()) {
#ifdef UNIX
    poller_ = new KqueuePoller(this);
#elsif LINUX
    //poller_ = new PollPoller(this);
#else
   // poller_ = new PollPoller(this);
#endif
    createWakeupfd();
}

EventLoop::~EventLoop() {}

void EventLoop::Run() {
    running_ = true;
    loop();
}

void EventLoop::Stop() {
    running_ = false;
}

void EventLoop::Quit() {
    //收尾工作
}

void EventLoop::AddTask(Functor cb) {
    if(isInLoopThread()) {
        cb();
    }else {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.push_back(std::move(cb));
    }
}

void EventLoop::AddTasks(std::vector<Functor>& cbs) {
    if(isInLoopThread()) {
        for(Functor cb : cbs) {
            cb();
        }
    }else {
        std::unique_lock<std::mutex> lock(mutex_);
        for(Functor cb : cbs) {
            tasks_.push_back(std::move(cb));
        }
    }
}

Timer* EventLoop::AddTimer(uint64_t ms, Functor cb, int repeats) {
    Timer* timer = new Timer(ms, cb, repeats);
    //线程安全
    timermanager_->AddTimer(timer);
    return timer;
}

void EventLoop::loop() {
    Time now = Time::Now();
    while(running_) {
        uint64_t timeout = timermanager_->NextTimeoutInterval();
        now = poller_->Poll(timeout, active_events_);
        handleActiveEvents(now);
        handleTasks();
        handleTimeoutTimers();
    }
}

void EventLoop::handleActiveEvents(Time time) {
    for(Event* event : active_events_) {
        event->HandleEvent(time);
    }
}

void EventLoop::handleTasks() {
    std::vector<Functor> tasks;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks.swap(tasks_);
    }
    
    for(Functor& task : tasks) {
        task();
    }
}

void EventLoop::handleTimeoutTimers() {
    timermanager_->ExecuteAllTimeoutTimer();
}

void EventLoop::createWakeupfd() {
    ::pipe(wakeup_fd_);
    wakeup_event_ = new Event(this, wakeup_fd_[0]);
    wakeup_event_->EnableReading();
    wakeup_event_->SetReadCallback(std::bind(&EventLoop::handleWakeup, this));
}

void EventLoop::wakeup() {
    char c = 'w';
    ::write(wakeup_fd_[1], &c, sizeof(c));
}

void EventLoop::handleWakeup() {
    char c;
    ::read(wakeup_fd_[0], &c, sizeof(c));
}


}
}
