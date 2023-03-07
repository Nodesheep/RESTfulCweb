#include "co_event.h"
#include "../tcpserver/timer.h"
#include "processer.h"
#include "coroutine.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

void CoEvent::HandleEvent(Time receiveTime) {
    if(revents_ & READ_EVENT) {
        if(read_coroutine_) {
            read_coroutine_->SetState(Coroutine::READY);
        }else {
            read_coroutine_ = new Coroutine([this, receiveTime](){
                read_callback_(receiveTime);
            });
            ((Processer*)loop_)->AddCoroutine(read_coroutine_);
        }
    }
    
    if(revents_ & WRITE_EVENT) {
        if(write_coroutine_) {
            write_coroutine_->SetState(Coroutine::READY);
        }else {
            write_coroutine_ = new Coroutine(write_callback_);
            ((Processer*)loop_)->AddCoroutine(write_coroutine_);
        }
    }
}

}
}
}
