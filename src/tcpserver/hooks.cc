#include "hooks.h"
#include "timer.h"
#include "singleton.h"
#include "pthread_keys.h"
#include "logger.h"
#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>
#include <iostream>
#include <sys/uio.h>
#include "event.h"
#ifdef COROUTINE
#include "coroutine.h"
#include "co_event.h"
#include "co_eventloop.h"
using namespace cweb::tcpserver::coroutine;
#endif

using namespace cweb::log;

namespace cweb {
namespace tcpserver {

typedef ssize_t (*read_fun)(int, void *, size_t);
typedef ssize_t (*readv_fun)(int fd, const struct iovec *iov, int iovcnt);
typedef ssize_t (*write_fun)(int, const void *, size_t);
typedef ssize_t (*writev_fun)(int fd, const struct iovec *iov, int iovcnt);
typedef int (*accept_fun)(int s, struct sockaddr *addr, socklen_t *addrlen);
typedef unsigned int (*sleep_fun)(unsigned int seconds);

template<typename OriginFun, typename... Args>
ssize_t io_handler(int fd, OriginFun fun, int type, Args&&... args) {
#ifdef COROUTINE
    CoEventLoop* TLSCoEventLoop = (CoEventLoop*)pthread_getspecific(util::PthreadKeysSingleton::GetInstance()->TLSEventLoop);
    
    if(!TLSCoEventLoop) {
        return fun(fd, std::forward<Args>(args)...);
    }
    
    CoEvent* event = TLSCoEventLoop->GetEvent(fd);
    
    if(!event) {
        return fun(fd, std::forward<Args>(args)...);
    }
    
    if(event->Flags() & O_NONBLOCK) {
        ssize_t n = fun(fd, std::forward<Args>(args)...);
        while(n == -1 && errno == EINTR) {
            n = fun(fd, std::forward<Args>(args)...);
        }
        if(n == -1 && errno == EAGAIN) {
            goto block;
        }
        return n;
    }else {
        goto block;
    }

block:
    Timer* timer = TLSCoEventLoop->AddTimer(60, [event](){
        event->HandleTimeout();
    });
    
    if(type == READ_EVENT) {
        if(!event->Readable()) {
            event->EnableReading();
            event->SetReadCoroutine(TLSCoEventLoop->GetCurrentCoroutine());
        }
    }else {
        if(!event->Writable()) {
            event->EnableWriting();
            event->SetWriteCoroutine(TLSCoEventLoop->GetCurrentCoroutine());
        }
    }
    
    TLSCoEventLoop->GetCurrentCoroutine()->SetState(Coroutine::HOLD);
    TLSCoEventLoop->GetCurrentCoroutine()->SwapTo(TLSCoEventLoop->GetMainCoroutine());
    
    //TODO 考虑timer事件和读写事件同时入队列的情况
    if(timer) {
        TLSCoEventLoop->RemoveTimer(timer);
    }
    
    return fun(fd, std::forward<Args>(args)...);
#else
    return fun(fd, std::forward<Args>(args)...);
#endif
}

int accept(int fd, struct sockaddr *addr, socklen_t *len) {
    static accept_fun accept_f = (accept_fun)dlsym(RTLD_NEXT, "accept");
#ifdef COROUTINE
    CoEventLoop* TLSCoEventLoop = (CoEventLoop*)pthread_getspecific(util::PthreadKeysSingleton::GetInstance()->TLSEventLoop);
    
    if(!TLSCoEventLoop) {
        return accept_f(fd, addr, len);
    }
    
    CoEvent* event = TLSCoEventLoop->GetEvent(fd);
    
    if(!event) {
        return accept_f(fd, addr, len);
    }
    
    if(event->Flags() & O_NONBLOCK) {
        ssize_t n = accept_f(fd, addr, len);
        while(n == -1 && errno == EINTR) {
            n = accept_f(fd, addr, len);
        }
        if(n == -1 && errno == EAGAIN) {
            goto block;
        }
        return (int)n;
    }else {
        goto block;
    }
block:
    if(!event->Readable()) {
        event->EnableReading();
        event->SetReadCoroutine(TLSCoEventLoop->GetCurrentCoroutine());
    }
    TLSCoEventLoop->GetCurrentCoroutine()->SetState(Coroutine::HOLD);
    TLSCoEventLoop->GetCurrentCoroutine()->SwapTo(TLSCoEventLoop->GetMainCoroutine());
#else
    return accept_f(fd, addr, len);
#endif
    return accept_f(fd, addr, len);
}

ssize_t read(int fd, void *buf, size_t nbyte) {
    static read_fun read_f = (read_fun)dlsym(RTLD_NEXT, "read");
    return io_handler(fd, read_f, READ_EVENT, buf, nbyte);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    static readv_fun readv_f = (readv_fun)dlsym(RTLD_NEXT, "readv");
    return io_handler(fd, readv_f, READ_EVENT, iov, iovcnt);
}

ssize_t write(int fd, const void *buf, size_t nbyte) {
    static write_fun write_f = (write_fun)dlsym(RTLD_NEXT, "write");
    return io_handler(fd, write_f, WRITE_EVENT, buf, nbyte);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    static writev_fun writev_f = (writev_fun)dlsym(RTLD_NEXT, "writev");
    return io_handler(fd, writev_f, WRITE_EVENT, iov, iovcnt);
}

unsigned int sleep(unsigned int seconds) {
    static sleep_fun sleep_f = (sleep_fun)dlsym(RTLD_NEXT, "sleep");
#ifdef COROUTINE
    CoEventLoop* TLSCoEventLoop = (CoEventLoop*)pthread_getspecific(util::PthreadKeysSingleton::GetInstance()->TLSEventLoop);
    Coroutine* co = TLSCoEventLoop->GetCurrentCoroutine();
    TLSCoEventLoop->AddTimer(seconds, [TLSCoEventLoop, co](){
        TLSCoEventLoop->AddCoroutineWithState(co);
    });
    TLSCoEventLoop->GetCurrentCoroutine()->SetState(Coroutine::HOLD);
    TLSCoEventLoop->GetCurrentCoroutine()->SwapTo(TLSCoEventLoop->GetMainCoroutine());
    return 0;
#else
    return sleep_f(seconds);
#endif
}

}
}
