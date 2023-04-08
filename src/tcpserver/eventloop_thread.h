#ifndef CWEB_TCP_EVENTLOOPTHREAD_H_
#define CWEB_TCP_EVENTLOOPTHREAD_H_

#include <pthread.h>
#include <mutex>
#include <string>

namespace cweb {
namespace tcpserver {

class EventLoop;
class EventLoopThread {
protected:
    EventLoop* loop_ = nullptr;
    bool stop_ = true;
    std::string name_;
    pthread_t tid_;
    std::mutex mutex_;
    std::condition_variable cond_;
    void createLoopAndRun();
    static void* threadFunc(void* arg);
    
public:
    EventLoopThread(const std::string& name = "") : name_(name) {};
    ~EventLoopThread();
    
    virtual EventLoop* StartLoop();
    virtual void StopLoop();
    std::string Name() const {return name_;}
};

}
}

#endif
