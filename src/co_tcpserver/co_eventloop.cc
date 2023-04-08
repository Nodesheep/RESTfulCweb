#include "co_eventloop.h"
#include "coroutine.h"
#include "co_event.h"
#include "timer.h"
#ifdef UNIX
#include "kqueue_poller.h"
#elsif LINUX
#include "epoll_poller.h"
#else
#include "poll_poller.h"
#endif
#include "pthread_keys.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

Coroutine* CoEventLoop::GetMainCoroutine() {
    return main_coroutine_;
}

void CoEventLoop::Run() {
    running_ = true;
    pthread_setspecific(util::PthreadKeysSingleton::GetInstance()->TLSEventLoop, this);
    pthread_setspecific(util::PthreadKeysSingleton::GetInstance()->TLSMemoryPool, memorypool_);
    main_coroutine_ = new Coroutine(std::bind(&CoEventLoop::loop, this));
    loop();
}

void CoEventLoop::AddTaskWithState(Functor cb, bool stateful) {
    Coroutine* co = new Coroutine(std::move(cb));
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if(stateful) {
            stateful_ready_coroutines_.Push(co);
        }else {
            stateless_ready_coroutines_.Push(co);
        }
    }
    if(!isInLoopThread()) {
        wakeup();
    }
}

void CoEventLoop::AddCoroutineWithState(Coroutine* co, bool stateful) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if(stateful) {
            stateful_ready_coroutines_.Push(co);
        }else {
            stateless_ready_coroutines_.Push(co);
        }
    }
    if(!isInLoopThread()) {
        wakeup();
    }
}

void CoEventLoop::AddTask(Functor cb) {
    Coroutine* co = new Coroutine(std::move(cb));
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stateful_ready_coroutines_.Push(co);
    }
    if(!isInLoopThread()) {
        wakeup();
    }
}

void CoEventLoop::AddTasks(std::vector<Functor>& cbs) {
    std::unique_lock<std::mutex> lock(mutex_);
    for(Functor cb : cbs) {
        Coroutine* co = new Coroutine(std::move(cb));
        stateful_ready_coroutines_.Push(co);
    }
    if(!isInLoopThread()) {
        wakeup();
    }
}

void CoEventLoop::UpdateEvent(Event *event) {
    events_[((CoEvent*)event)->fd_] = (CoEvent*)event;
    poller_->UpdateEvent(event);
}

void CoEventLoop::RemoveEvent(Event *event) {
    events_.erase(((CoEvent*)event)->fd_);
    poller_->RemoveEvent(event);
}

CoEvent* CoEventLoop::GetEvent(int fd) {
    //return (CoEvent*)events_[fd];
    if(events_.find(fd) == events_.end()) {
        return nullptr;
    }
    return (CoEvent*)events_[fd];
}

Coroutine* CoEventLoop::GetCurrentCoroutine() {
    return running_coroutine_;
}

void CoEventLoop::NotifyCoroutineReady(Coroutine *co) {
    assert(isInLoopThread());
    hold_coroutines_.Erase(co);
    running_coroutines_.Push(co);
}

//主协程
void CoEventLoop::loop() {
    while(running_) {
        //epoll
        active_events_.clear();
        Time now = poller_->Poll(-1, active_events_);
  
        handleActiveEvents(now);
        handleTimeoutTimers();
            
        running_coroutine_ = running_coroutines_.Front();
        if(!running_coroutine_) {
            moveReadyCoroutines();
            running_coroutine_ = running_coroutines_.Front();
        }
        
        while(running_coroutine_ && running_) {
            //running_coroutine_->SwapIn();
            running_coroutine_->SetLoop(this);
            main_coroutine_->SwapTo(running_coroutine_);
            switch (running_coroutine_->State()) {
                case Coroutine::State::EXEC: {
                    next_coroutine_ = running_coroutines_.Next(running_coroutine_);
                    if(!next_coroutine_) {
                        moveReadyCoroutines();
                        next_coroutine_ = running_coroutines_.Next(running_coroutine_);
                    }
                    
                    running_coroutine_ = next_coroutine_;
                }
                    break;
                case Coroutine::State::HOLD: {
                    //移动到阻塞队列
                    //可执行队列、阻塞队列、不可抢占等待队列、可抢占等待队列
                    running_coroutines_.Erase(running_coroutine_);
                    hold_coroutines_.Push(running_coroutine_);
                    next_coroutine_ = running_coroutines_.Next(running_coroutine_);
                    if(!next_coroutine_) {
                        moveReadyCoroutines();
                        next_coroutine_ = running_coroutines_.Next(running_coroutine_);
                    }
                    running_coroutine_ = next_coroutine_;
                }
                    
                    break;
                    
                case Coroutine::State::TERM:
                default: {
                    next_coroutine_ = running_coroutines_.Next(running_coroutine_);
                    if(!next_coroutine_) {
                        moveReadyCoroutines();
                        next_coroutine_ = running_coroutines_.Next(running_coroutine_);
                    }
                    
                    running_coroutines_.Erase(running_coroutine_);
                    running_coroutine_->Remove();
                    delete running_coroutine_;
                    running_coroutine_ = next_coroutine_;
                    
                }
                    break;
            }
        }
    }
}

void CoEventLoop::handleActiveEvents(Time time) {
    for(Event* event : active_events_) {
        event->HandleEvent(time);
    }
}

void CoEventLoop::handleTimeoutTimers() {
    std::vector<Functor> timeouts;
    if(timermanager_->PopAllTimeoutFunctor(timeouts)) {
        AddTasks(timeouts);
    }
}

void CoEventLoop::moveReadyCoroutines() {
    std::unique_lock<std::mutex> lock(mutex_);
    size_t size = stateful_ready_coroutines_.Size();
    if(size > 0) {
        running_coroutines_.Push(stateful_ready_coroutines_);
    }
    
    if(size < 5 && stateless_ready_coroutines_.Size() > 0) {
        running_coroutines_.Push(stateless_ready_coroutines_);
    }
}

}
}
}