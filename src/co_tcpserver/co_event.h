#ifndef CWEB_COROUTINE_COEVENT_H_
#define CWEB_COROUTINE_COEVENT_H_

#include "../tcpserver/event.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

class Coroutine;
class CoEvent : public Event {
    
public:
    virtual void HandleEvent(Time receiveTime) override;
    
private:
    Coroutine* read_coroutine_ = nullptr;
    Coroutine* write_coroutine_ = nullptr;
};

}
}
}

#endif
