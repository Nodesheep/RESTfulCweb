#include "processer.h"
#include "coroutine.h"
#include "../tcpserver/timer.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {
static __thread Coroutine* TLSProccserMainCoroutine = NULL;

Coroutine* Processer::GetMainCoroutine() {
    return TLSProccserMainCoroutine;
}

void Processer::Run() {
    running_ = true;
    loop();
}

void Processer::AddTask(Functor cb) {
    Coroutine* co = new Coroutine(std::move(cb));
    std::unique_lock<std::mutex> lock(mutex_);
    ready_coroutines_.PushBackLockFree(co);
    if(!isInLoopThread()) {
        wakeup();
    }
}

void Processer::AddCoroutine(Coroutine* co) {
    std::unique_lock<std::mutex> lock(mutex_);
    ready_coroutines_.PushBackLockFree(co);
    if(!isInLoopThread()) {
        wakeup();
    }
}

void Processer::AddTasks(std::vector<Functor>& cbs) {
    std::unique_lock<std::mutex> lock(mutex_);
    for(Functor cb : cbs) {
        Coroutine* co = new Coroutine(std::move(cb));
        ready_coroutines_.PushBackLockFree(co);
    }
    if(!isInLoopThread()) {
        wakeup();
    }
}

//主协程
//要避免重复加入协程 应当event中有一个标志位
void Processer::loop() {
    while(running_) {
        //epoll
        //避免重复添加读写协程
        //timeout
        std::vector<Functor> timeouts;
        if(timermanager_->PopAllTimeoutFunctor(timeouts)) {
            AddTasks(timeouts);
        }
            
        ready_coroutines_.FrontLockFree(running_coroutine_);
        
        while(running_coroutine_ && running_) {
            running_coroutine_->SwapIn();
            switch (running_coroutine_->State()) {
                case Coroutine::State::EXEC: {
                    ready_coroutines_.Next(running_coroutine_, next_coroutine_);
                    if(!next_coroutine_) {
                        moveCoroutines();
                        ready_coroutines_.Next(running_coroutine_, next_coroutine_);
                    }
                    
                    running_coroutine_ = next_coroutine_;
                }
                    break;
                case Coroutine::State::HOLD: {
                    ready_coroutines_.Next(running_coroutine_, next_coroutine_);
                    if(!next_coroutine_) {
                        moveCoroutines();
                        ready_coroutines_.Next(running_coroutine_, next_coroutine_);
                    }
                    running_coroutine_ = next_coroutine_;
                }
                    
                    break;
                    
                case Coroutine::State::TERM:
                default: {
                    ready_coroutines_.Next(running_coroutine_, next_coroutine_);
                    if(!next_coroutine_) {
                        moveCoroutines();
                        ready_coroutines_.Next(running_coroutine_, next_coroutine_);
                    }
                    
                    ready_coroutines_.Erase(running_coroutine_);
                    running_coroutine_ = next_coroutine_;
                }
                    break;
            }
        }
    }
}

void Processer::moveCoroutines() {
    
}

}
}
}
