#include "coroutine.h"
#include "co_eventloop.h"
#include "coroutine_context.h"
#include "pthread_keys.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

static const size_t kCoroutineContextSize = 40 * 1024;

void Coroutine::coroutineFunc(void *vp) {
    Coroutine* co = (Coroutine*)vp;
    co->run();
}

Coroutine::Coroutine(std::function<void()> func, CoEventLoop* loop) : func_(std::move(func)), context_(new CoroutineContext(kCoroutineContextSize, coroutineFunc, this)), loop_(loop) {}

Coroutine::~Coroutine() {
    delete context_;
}

void Coroutine::SwapIn() {
    state_ = EXEC;
    CoroutineContext::ContextSwap(((CoEventLoop*)pthread_getspecific(util::PthreadKeysSingleton::GetInstance()->TLSEventLoop))->GetMainCoroutine()->context_, context_);
    //CoroutineContext::ContextSwap(CoEventLoop::GetMainCoroutine()->context_, context_);
}

void Coroutine::SwapOut() {
    CoroutineContext::ContextSwap(context_, ((CoEventLoop*)pthread_getspecific(util::PthreadKeysSingleton::GetInstance()->TLSEventLoop))->GetMainCoroutine()->context_);
   // CoroutineContext::ContextSwap(context_, CoEventLoop::GetMainCoroutine()->context_);
}

void Coroutine::SwapTo(Coroutine *co) {
    CoroutineContext::ContextSwap(context_, co->context_);
}

void Coroutine::SetState(enum State state) {
    if(state_ != READY && state == READY) {
        loop_->NotifyCoroutineReady(this);
    }
    state_ = state;
}

void Coroutine::run() {
    if(func_) func_();
    state_ = TERM;
    //SwapOut(pthread_getspecific(<#pthread_key_t#>));
    CoroutineContext::ContextSwap(context_, ((CoEventLoop*)pthread_getspecific(util::PthreadKeysSingleton::GetInstance()->TLSEventLoop))->GetMainCoroutine()->context_);
}

}
}
}
