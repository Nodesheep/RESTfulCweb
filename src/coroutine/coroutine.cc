#include "coroutine.h"
#include "processer.h"
#include "coroutine_context.h"

namespace cweb {
namespace coroutine {

static const size_t kCoroutineContextSize = 4096;

Coroutine::Coroutine(void (*fn)()) : context_(new CoroutineContext(kCoroutineContextSize, fn)) {}

void Coroutine::SwapIn() {
    state_ = EXEC;
    CoroutineContext::ContextSwap(Processer::GetMainCoroutine()->context_, context_);
}

void Coroutine::SwapOut() {
    CoroutineContext::ContextSwap(context_, Processer::GetMainCoroutine()->context_);
}

}
}
