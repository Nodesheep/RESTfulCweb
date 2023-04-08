#include "coroutine_context.h"

#include <string.h>

/*
#define R15 0
#define R14 1
#define R13 2
#define R12 3
//#define R9 4

//#define R8 5
#define RBP 4
//#define RDI 7
//#define RSI 8
#define RET 5
//#define RDX 10
//#define RCX 11
#define RBX 6
#define RSP 7
#define RDI 8*/


#define RSP 0
#define RIP 1
#define RBX 2
#define RDI 3
#define RSI 4

#define RBP 5
#define R12 6
#define R13 7
#define R14 8
#define R15 9
#define RDX 10
#define RCX 11
#define R8 12
#define R9 13

enum {
  kEIP = 0,
  kEBP = 6,
  kESP = 7,
};

enum {
  kRDI = 7,
  kRSI = 8,
  kRETAddr = 9,
  kRSP = 13,
};

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
    /*ss_sp = (char*)malloc(size); //堆 低->高
    //移动到高地址 栈 高->低
    char* sp = ss_sp + ss_size - sizeof(void*);
    
    //对齐 移动（16 - ss_size % 16）
    sp = (char*)((unsigned long)sp & -16LL);
    
    memset(regs, 0, sizeof(regs));
    
    //
    void** ret_addr = (void**)(sp);
    *ret_addr = (void*)fn;
    //*(void**)sp = (void*)fn;
    
    //初始时栈顶位置
    regs[RBP] = sp + sizeof(void*);
    regs[RSP] = sp; //栈顶指针存储函数地址 函数运行过程栈指针向高地址增长
    regs[RDI] = (void*)vp;
    
    regs[RET] = (void*)fn; //函数返回地址
*/
    
    
    ss_sp = (char*)malloc(size);
    char* sp = ss_sp + ss_size - sizeof(void*);
    sp = (char*)((unsigned long)sp & -16LL);

    memset(regs, 0, sizeof(regs));
    void** ret_addr = (void**)(sp);
    *ret_addr = (void*)fn;

    regs[kRSP] = sp;

    regs[kRETAddr] = (char*)fn;

    regs[kRDI] = (char*)vp;
   // regs[kRSI] = (char*)s1;
}

void CoroutineContext::ContextSwap(CoroutineContext *from, CoroutineContext *to) {
    context_swap(from, to);
}

}
}
}
