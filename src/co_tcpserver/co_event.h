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
    CoEvent(std::shared_ptr<CoEventLoop> loop, int fd, bool is_socket = false);
    
    virtual ~CoEvent();
    
    virtual void HandleEvent(Time receiveTime) override;
    
    void SetReadCoroutine(Coroutine* co);
    void SetWriteCoroutine(Coroutine* co);
    void TriggerEvent();
    bool Triggred() {return triggered_;}
private:
    bool triggered_ = false;
    int flags_ = 0;
    Coroutine* read_coroutine_ = nullptr;
    Coroutine* write_coroutine_ = nullptr;
};

}
}
}

#endif
