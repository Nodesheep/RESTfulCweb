#include "co_event.h"
#include "timer.h"
#include "co_eventloop.h"
#include "coroutine.h"
#include "co_eventloop.h"
#include <fcntl.h>

namespace cweb {
namespace tcpserver {
namespace coroutine {

CoEvent::CoEvent(CoEventLoop* loop, int fd)
: Event(loop, fd) {
    flags_ = ::fcntl(fd_, F_GETFL, 0);
}

void CoEvent::HandleEvent(Time receiveTime) {
    if(revents_ & READ_EVENT) {
        if(read_coroutine_) {
            read_coroutine_->SetState(Coroutine::READY);
        }else {
            read_coroutine_ = new Coroutine([this, receiveTime](){
                read_callback_(receiveTime);
            });
            read_coroutine_->SetEvent(this);
            ((CoEventLoop*)loop_)->AddCoroutineWithState(read_coroutine_);
        }
    }
    
    if(revents_ & WRITE_EVENT) {
        if(write_coroutine_) {
            write_coroutine_->SetState(Coroutine::READY);
        }else {
            write_coroutine_ = new Coroutine(write_callback_);
            write_coroutine_->SetEvent(this);
            ((CoEventLoop*)loop_)->AddCoroutineWithState(write_coroutine_);
        }
    }
}

void CoEvent::ExecuteReadCoroutine() {
    if(read_coroutine_) {
        read_coroutine_->SetState(Coroutine::READY);
    }else {
        Time now = Time::Now();
        read_coroutine_ = new Coroutine([this, now](){
            read_callback_(now);
        });
        read_coroutine_->SetEvent(this);
        ((CoEventLoop*)loop_)->AddCoroutineWithState(read_coroutine_);
    }
}

void CoEvent::ExecuteWriteCoroutine() {
    if(write_coroutine_) {
        write_coroutine_->SetState(Coroutine::READY);
    }else {
        write_coroutine_ = new Coroutine(write_callback_);
        write_coroutine_->SetEvent(this);
        ((CoEventLoop*)loop_)->AddCoroutineWithState(write_coroutine_);
    }
}

void CoEvent::RemoveCoroutine(Coroutine *co) {
    if(co == read_coroutine_) {
        read_coroutine_ = nullptr;
    }else if(co == write_coroutine_) {
        write_coroutine_ = nullptr;
    }
}

}
}
}
