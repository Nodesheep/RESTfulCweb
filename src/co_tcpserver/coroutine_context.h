#ifndef CWEB_COROUTINE_COROUTINECONTEXT_H_
#define CWEB_COROUTINE_COROUTINECONTEXT_H_

//目前只支持x86_64
#include <stdlib.h>
#include <stddef.h>

namespace cweb {
namespace tcpserver {
namespace coroutine {

class CoroutineContext {
public:
    void* regs[9];
    size_t ss_size = 0;
    char* ss_sp = nullptr;
    
    CoroutineContext();
    ~CoroutineContext();
    CoroutineContext(size_t size, void (*fn)(void*), const void* vp);
    void Init(size_t size, void (*fn)(void*), const void* vp);
    static void ContextSwap(CoroutineContext *from, CoroutineContext *to);
};

}
}
}

#endif
