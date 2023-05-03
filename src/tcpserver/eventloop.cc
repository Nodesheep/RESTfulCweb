#include "eventloop.h"
#include "event.h"
#include "timer.h"
#ifdef KQUEUE
#include "kqueue_poller.h"
#elif EPOLL
#include "epoll_poller.h"
#else
#include "poll_poller.h"
#endif
#include "pthread_keys.h"

#include <unistd.h>

namespace cweb {
namespace tcpserver {

EventLoop::EventLoop()
:tid_(pthread_self()) {
#ifdef KQUEUE
    poller_ = new KqueuePoller(this);
#elif EPOLL
    poller_ = new EPollPoller(this);
#else
    poller_ = new PollPoller(this);
#endif
    timermanager_ = new TimerWheelManager();
    memorypool_ = new util::MemoryPool();
    createWakeupfd();
}

EventLoop::~EventLoop() {
    delete poller_;
    delete memorypool_;
    delete timermanager_;
}

void EventLoop::Run() {
    pthread_setspecific(util::PthreadKeysSingleton::GetInstance()->TLSMemoryPool, memorypool_);
    running_ = true;
    loop();
}

void EventLoop::Quit() {
    running_ = false;
    if(!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::AddTask(Functor cb) {
    if(isInLoopThread()) {
        cb();
    }else {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.push_back(std::move(cb));
        wakeup();
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

Timer* EventLoop::AddTimer(uint64_t s, Functor cb, int repeats) {
    Timer* timer = new Timer(s, cb, repeats);
    //线程安全
    timermanager_->AddTimer(timer);
    return timer;
}

void EventLoop::RemoveTimer(Timer *timer) {
    timermanager_->RemoveTimer(timer);
}

void EventLoop::UpdateEvent(Event *event) {
    poller_->UpdateEvent(event);
}

void EventLoop::RemoveEvent(Event *event) {
    poller_->RemoveEvent(event);
}

void EventLoop::loop() {
    Time now = Time::Now();
    while(running_) {
        active_events_.clear();
        int timeout = timermanager_->NextTimeoutInterval();
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
    std::vector<Timer*> timeouts;
    if(timermanager_->PopAllTimeoutTimer(timeouts)) {
        for(Timer* timeout : timeouts) {
            timeout->Execute();
        }
    }
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
