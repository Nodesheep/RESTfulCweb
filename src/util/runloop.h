#ifndef CWEB_UTIL_RUNLOOP_H_
#define CWEB_UTIL_RUNLOOP_H_

#include "noncopyable.h"

namespace cweb {
namespace util {

class RunLoop : public Noncopyable {
protected:
    bool running_ = false;
    virtual void loop() = 0;
    virtual void wakeup() = 0;
    
public:
    virtual void Run() = 0;
    virtual void Stop() = 0;
    virtual void Quit() = 0;
};

}
}

#endif
