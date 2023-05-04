#ifndef CWEB_COROUTINE_COEVENT_H_
#define CWEB_COROUTINE_COEVENT_H_

#include "event.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

class CoEventLoop;
class Coroutine;
class CoEvent : public Event {
    
public:
    friend CoEventLoop;
    CoEvent(CoEventLoop* loop, int fd);
    virtual ~CoEvent();
    virtual void HandleEvent(Time receiveTime) override;
    
    int Flags() const {return flags_;}
    void RemoveCoroutine(Coroutine* co);
    void SetReadCoroutine(Coroutine* co);
    void SetWriteCoroutine(Coroutine* co);
    
private:
    int flags_ = 0;
    Coroutine* read_coroutine_ = nullptr;
    Coroutine* write_coroutine_ = nullptr;
};

}
}
}

#endif
