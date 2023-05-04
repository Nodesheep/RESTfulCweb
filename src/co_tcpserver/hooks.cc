#include "hooks.h"
#include "coroutine.h"
#include "timer.h"
#include "co_event.h"
#include "co_eventloop.h"
#include "../util/singleton.h"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "pthread_keys.h"
#include "logger.h"

using namespace cweb::log;

namespace cweb {
namespace tcpserver {
namespace coroutine {

int hook_accept(int fd, struct sockaddr *addr, socklen_t *len) {
    CoEventLoop* TLSCoEventLoop = (CoEventLoop*)pthread_getspecific(util::PthreadKeysSingleton::GetInstance()->TLSEventLoop);
    
    CoEvent* event = TLSCoEventLoop->GetEvent(fd);
    if(!event || event->Flags() & O_NONBLOCK) {
        return  ::accept(fd, addr, len);
    }
    
    if(!event->Readable()) event->EnableReading();
    
    TLSCoEventLoop->GetCurrentCoroutine()->SetState(Coroutine::HOLD);
    TLSCoEventLoop->GetCurrentCoroutine()->SwapTo(TLSCoEventLoop->GetMainCoroutine());
    
    
    return ::accept(fd, addr, len);
}

ssize_t hook_read(int fd, void *buf, size_t nbyte) {

    CoEventLoop* TLSCoEventLoop = (CoEventLoop*)pthread_getspecific(util::PthreadKeysSingleton::GetInstance()->TLSEventLoop);
    
    if(TLSCoEventLoop == 0) {
        return ::read(fd, buf, nbyte);
    }
    
    CoEvent* event = TLSCoEventLoop->GetEvent(fd);
    
    //TODO hook写的有问题，应当是用户非阻塞！！
    if(!event || event->Flags() & O_NONBLOCK) {
        return ::read(fd, buf, nbyte);
    }
    
    Timer* timer = TLSCoEventLoop->AddTimer(60, [event](){
        event->HandleTimeout();
    });
    
    event->SetReadCoroutine(TLSCoEventLoop->GetCurrentCoroutine());
    if(!event->Readable()) event->EnableReading();
    
    TLSCoEventLoop->GetCurrentCoroutine()->SetState(Coroutine::HOLD);
    TLSCoEventLoop->GetCurrentCoroutine()->SwapTo(TLSCoEventLoop->GetMainCoroutine());
    
    if(timer) {
        TLSCoEventLoop->RemoveTimer(timer);
    }
    
    return ::read(fd, buf, nbyte);
}

ssize_t hook_write(int fd, const void *buf, size_t nbyte) {
    //fd与event的映射
    
    CoEventLoop* TLSCoEventLoop = (CoEventLoop*)pthread_getspecific(util::PthreadKeysSingleton::GetInstance()->TLSEventLoop);
    
    CoEvent* event = TLSCoEventLoop->GetEvent(fd);
    if(!event || event->Flags() & O_NONBLOCK) {
        return ::write(fd, buf, nbyte);
    }
    
    //避免重复添加事件
    event->SetWriteCoroutine(TLSCoEventLoop->GetCurrentCoroutine());
    if(!event->Writable()) event->EnableWriting();
    
    TLSCoEventLoop->GetCurrentCoroutine()->SetState(Coroutine::HOLD);
    TLSCoEventLoop->GetCurrentCoroutine()->SwapTo(TLSCoEventLoop->GetMainCoroutine());
    
    return ::write(fd, buf, nbyte);
}


}
}
}
