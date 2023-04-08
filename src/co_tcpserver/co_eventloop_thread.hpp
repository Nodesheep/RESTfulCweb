#ifndef CWEB_COROUTINE_COEVENTLOOPTHREAD_H_
#define CWEB_COROUTINE_COEVENTLOOPTHREAD_H_

#include <thread>
#include <mutex>
#include <string>

namespace cweb {
namespace tcpserver {

class EventLoop;
class EventLoopThread {
private:
    EventLoop* loop_ = nullptr;
    bool stop_ = true;
    std::string name_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    void createLoopAndRun();
    
public:
    EventLoopThread(const std::string& name = "") : name_(name) {};
    ~EventLoopThread();
    
    EventLoop* StartLoop();
    void StopLoop();
    std::string Name() const {return name_;}
};

}
}

#endif

