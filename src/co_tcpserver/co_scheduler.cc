#include "co_scheduler.h"
#include "co_eventloop.h"
#include "co_eventloop_thread.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

CoScheduler::CoScheduler(std::shared_ptr<CoEventLoop> baseloop, int threadcnt)
: Scheduler(baseloop, threadcnt) {}

void CoScheduler::Start() {
    for(int i = 0; i < threadcnt_; ++i) {
        std::unique_ptr<CoEventLoopThread> thread(new CoEventLoopThread());
        loops_.push_back(thread->StartLoop());
        threads_.push_back(std::move(thread));
    }
}

}
}
}
