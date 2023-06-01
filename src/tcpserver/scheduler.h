#ifndef CWEB_TCP_SCHEDULER_H_
#define CWEB_TCP_SCHEDULER_H_

#include <vector>
#include <memory>
#include "noncopyable.h"

namespace cweb {
namespace tcpserver {

class EventLoop;
class EventLoopThread;
class Scheduler : public util::Noncopyable {
protected:
    std::shared_ptr<EventLoop> baseloop_;
    int next_ = -1;
    int threadcnt_ = 0;
    std::vector<std::shared_ptr<EventLoop>> loops_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    
public:
    Scheduler(std::shared_ptr<EventLoop> baseloop, int threadcnt);
    ~Scheduler();
    std::shared_ptr<EventLoop> GetNextLoop();
    virtual void Start();
    virtual void Stop();
};

}
}

#endif
