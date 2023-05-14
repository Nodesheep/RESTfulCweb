#include "scheduler.h"
#include "eventloop_thread.h"
#include "eventloop.h"

namespace cweb {
namespace tcpserver {

Scheduler::Scheduler(std::shared_ptr<EventLoop> baseloop, int threadcnt) : baseloop_(baseloop), threadcnt_(threadcnt) {}

Scheduler::~Scheduler() {
    //不要回收loop 外部创建的临时变量
}

void Scheduler::Start() {
    for(int i = 0; i < threadcnt_; ++i) {
        std::unique_ptr<EventLoopThread> thread(new EventLoopThread());
        loops_.push_back(thread->StartLoop());
        threads_.push_back(std::move(thread));
    }
}

void Scheduler::Stop() {
    for (auto loop : loops_) {
        loop->Quit();
    }
}

std::shared_ptr<EventLoop> Scheduler::GetNextLoop() {
    ++next_;
    next_ %= threadcnt_;
    //单线程
    return loops_[next_];
}

}
}
