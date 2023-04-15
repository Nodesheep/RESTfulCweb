#include "coroutine_context.h"
#include <string.h>

#define R15 0
#define R14 1
#define R13 2
#define R12 3
#define RBP 4
#define RET 5
#define RBX 6
#define RSP 7
#define RDI 8

namespace cweb {
namespace tcpserver {
namespace coroutine {

void context_swap(CoroutineContext* from, CoroutineContext* to);

CoroutineContext::CoroutineContext() {}

CoroutineContext::CoroutineContext(size_t size, void (*fn)(void*), const void* vp) : ss_size(size) {
    Init(size, fn, vp);
}

CoroutineContext::~CoroutineContext() {
    free(ss_sp);
}

void CoroutineContext::Init(size_t size, void (*fn)(void*), const void* vp) {
    ss_sp = (char*)malloc(size); //堆 低->高
    //移动到高地址 栈 高->低
    char* sp = ss_sp + ss_size - sizeof(void*);
    
    //对齐 移动（16 - ss_size % 16）
    sp = (char*)((unsigned long)sp & -16LL);
    
    memset(regs, 0, sizeof(regs));
    
    *(void**)sp = (void*)fn;
    
    //初始时栈顶位置
    regs[RBP] = sp + sizeof(void*);
    regs[RSP] = sp; //栈顶指针存储函数地址 函数运行过程栈指针向高地址增长
    regs[RDI] = (void*)vp;
    
    regs[RET] = (void*)fn; //函数返回地址
}

void CoroutineContext::ContextSwap(CoroutineContext *from, CoroutineContext *to) {
    context_swap(from, to);
}

}
}
}
