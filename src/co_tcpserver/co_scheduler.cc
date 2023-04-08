#include "co_scheduler.h"
#include "co_eventloop.h"
#include "co_eventloop_thread.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

CoScheduler::CoScheduler(CoEventLoop* baseloop, int threadcnt)
: Scheduler(baseloop, threadcnt) {}

void CoScheduler::Start() {
    for(int i = 0; i < threadcnt_; ++i) {
        CoEventLoopThread* thread = new CoEventLoopThread();
        loops_.push_back(thread->StartLoop());
        threads_.push_back(thread);
    }
}

}
}
}
