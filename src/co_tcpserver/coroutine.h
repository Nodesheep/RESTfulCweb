#ifndef CWEB_COROUTINE_COROUTINE_H_
#define CWEB_COROUTINE_COROUTINE_H_

#include <vector>
#include <functional>

namespace cweb {
namespace tcpserver {
namespace coroutine {

class CoroutineContext;
class Processer;
class Coroutine {
    
public:
    enum State {
        READY,
        HOLD,
        EXEC,
        TERM
    };
    
    Coroutine(std::function<void()> func);
    void SwapIn();
    void SwapOut();
    
    void SetState(State state) {state_ = state;}
    State State() const {return state_;}
    
private:
    enum State state_ = READY;
    CoroutineContext* context_;
    void* stack_ptr_;
    std::function<void()> func_;
    Processer* processer_;
    
    void run();
    static void contextFunc(void* vp);
};

}
}
}

#endif
