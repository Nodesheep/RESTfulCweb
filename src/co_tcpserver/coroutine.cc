#include "coroutine.h"
#include "processer.h"
#include "coroutine_context.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

static const size_t kCoroutineContextSize = 4096;

void Coroutine::contextFunc(void *vp) {
    Coroutine* co = (Coroutine*)vp;
    co->run();
}

Coroutine::Coroutine(std::function<void()> func) : func_(std::move(func)), context_(new CoroutineContext(kCoroutineContextSize, &Coroutine::contextFunc, this)) {}

void Coroutine::SwapIn() {
    state_ = EXEC;
    CoroutineContext::ContextSwap(Processer::GetMainCoroutine()->context_, context_);
}

void Coroutine::SwapOut() {
    CoroutineContext::ContextSwap(context_, Processer::GetMainCoroutine()->context_);
}

void Coroutine::run() {
    if(func_) func_();
}

}
}
}
