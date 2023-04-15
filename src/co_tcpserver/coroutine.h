#ifndef CWEB_COROUTINE_COROUTINE_H_
#define CWEB_COROUTINE_COROUTINE_H_

#include <vector>
#include <functional>
#include "linked_list.h"
#include "co_event.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

class CoroutineContext;
class CoEventLoop;
class CoEvent;
class Coroutine : public util::LinkedListNode {
    
public:
    enum State {
        READY,
        HOLD,
        EXEC,
        TERM,
        REMOVE
    };
    
    Coroutine(std::function<void()> func, CoEventLoop* loop = nullptr);
    ~Coroutine();
    void SwapIn();
    void SwapOut();
    void SwapTo(Coroutine* co);
    
    void SetState(State state);
    State State() const {return state_;}
    void SetLoop(CoEventLoop* loop) {loop_ = loop;}
    
    void SetEvent(CoEvent* event) {event_ = event;}
    void Remove() {
        if(event_) {
            event_->RemoveCoroutine(this);
        }
    }
    
private:
    enum State state_ = READY;
    CoroutineContext* context_;
    std::function<void()> func_;
    CoEventLoop* loop_;
    CoEvent* event_ = nullptr;
    void run();
    static void coroutineFunc(void* vp);
};

}
}
}

#endif
