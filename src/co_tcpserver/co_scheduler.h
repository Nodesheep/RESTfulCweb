#ifndef CWEB_COROUTINE_COSCHEDULER_H_
#define CWEB_COROUTINE_COSCHEDULER_H_

#include "noncopyable.h"
#include "scheduler.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

class Coroutine;
class CoEventLoop;
class CoScheduler : public Scheduler {
private:

public:
    CoScheduler(CoEventLoop* baseloop, int threadcnt);
    virtual void Start() override;
    
};

}
}
}

#endif
