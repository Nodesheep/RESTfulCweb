#ifndef CWEB_COROUTINE_COEVENTLOOPTHREAD_H_
#define CWEB_COROUTINE_COEVENTLOOPTHREAD_H_

#include <mutex>
#include <string>
#include "eventloop_thread.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

class CoEventLoopThread : public EventLoopThread {
protected:
    void createLoopAndRun();
    static void* cothreadFunc(void* arg);
    
public:
    CoEventLoopThread(const std::string& name = "") : EventLoopThread(name) {};
    ~CoEventLoopThread();
    
    virtual EventLoop* StartLoop() override;
    //virtual void StopLoop() override;
};

}
}
}

#endif

