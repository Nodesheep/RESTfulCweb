#include "co_eventloop_thread.h"
#include "co_eventloop.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

EventLoop* CoEventLoopThread::StartLoop() {
    stop_ = false;
    pthread_create(&tid_, NULL, cothreadFunc, this);
    
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(loop_ == nullptr) {
            cond_.wait(lock);
        }
    }
    
    return loop_;
}

void* CoEventLoopThread::cothreadFunc(void *arg) {
    CoEventLoopThread* thread = (CoEventLoopThread*)arg;
    thread->createLoopAndRun();
    return NULL;
}

void CoEventLoopThread::createLoopAndRun() {
    CoEventLoop loop;
    
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_all();
    }
    loop.Run();
}

}
}
}
