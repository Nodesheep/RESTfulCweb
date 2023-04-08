#ifndef CWEB_TCP_SCHEDULER_H_
#define CWEB_TCP_SCHEDULER_H_

#include <vector>
#include "noncopyable.h"

namespace cweb {
namespace tcpserver {

class EventLoop;
class EventLoopThread;
class Scheduler : public util::Noncopyable {
protected:
    EventLoop* baseloop_ = nullptr;
    int next_ = -1;
    int threadcnt_ = 0;
    std::vector<EventLoop*> loops_;
    std::vector<EventLoopThread*> threads_;
    
public:
    Scheduler(EventLoop* baseloop, int threadcnt);
    ~Scheduler();
    EventLoop* GetNextLoop();
    virtual void Start();
    virtual void Stop();
};

}
}

#endif
