#ifndef CWEB_COROUTINE_COROUTINE_H_
#define CWEB_COROUTINE_COROUTINE_H_

#include <vector>
#include <functional>

namespace cweb {
namespace coroutine {

class CoroutineContext;
class Coroutine {

public:
    enum State {
        INIT,
        HOLD,
        EXEC,
        TERM
    };

    Coroutine(void (*fn)());
    void SwapIn();
    void SwapOut();
    State State() {return state_;}

private:
    enum State state_ = INIT;
    CoroutineContext* context_;
    void* stack_ptr_;
    std::function<void()> func_;
};


}
}

#endif
