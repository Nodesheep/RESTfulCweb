#include "eventloop_thread.h"
#include "eventloop.h"

namespace cweb {
namespace tcpserver {

EventLoopThread::~EventLoopThread() {
    if(!stop_) {
        StopLoop();
    }
}

EventLoop* EventLoopThread::StartLoop() {
    stop_ = false;
    pthread_create(&tid_, NULL, threadFunc, this);
    //thread_ = std::thread(std::bind(&EventLoopThread::createLoopAndRun, this));
    
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(loop_ == nullptr) {
            cond_.wait(lock);
        }
    }

    return loop_;
}

void EventLoopThread::StopLoop() {
    stop_ = true;
    if(loop_ != nullptr) {
        loop_->Quit();
        pthread_join(tid_, NULL);
    }
}

void* EventLoopThread::threadFunc(void* arg) {
    EventLoopThread* thread = (EventLoopThread*)arg;
    thread->createLoopAndRun();
    return NULL;
}

void EventLoopThread::createLoopAndRun() {
    EventLoop loop;
    
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_all();
    }
    
    loop.Run();
}


}
}
