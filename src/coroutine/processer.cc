#include "processer.h"
#include "coroutine.h"

namespace cweb {
namespace coroutine {
static __thread Coroutine* TLSProccserMainCoroutine = nullptr;

Coroutine* Processer::GetMainCoroutine() {
    return TLSProccserMainCoroutine;
}

void Processer::Run() {
    running_ = true;
    loop();
}
//主协程
//要避免重复加入协程 应当event中有一个标志位
void Processer::loop() {
    while(running_) {
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
