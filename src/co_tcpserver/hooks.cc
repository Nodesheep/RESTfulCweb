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

namespace cweb {
namespace tcpserver {
namespace coroutine {

int hook_accept(int fd, struct sockaddr *addr, socklen_t *len) {
    CoEventLoop* TLSCoEventLoop = (CoEventLoop*)pthread_getspecific(util::PthreadKeysSingleton::GetInstance()->TLSEventLoop);
    //std::cout << TLSCoEventLoop << std::endl;
    
    CoEvent* event = TLSCoEventLoop->GetEvent(fd);
    if(!event || event->Flags() & O_NONBLOCK) {
        return  ::accept(fd, addr, len);
    }
    
    //int timeout = 0;
    //Timer* timer = TLSCoEventLoop->AddTimer(timeout, [event](){
        //event->disable
        //event->Remove();
        //close connection
    //});
    //避免重复添加事件
    if(!event->Readable()) event->EnableReading();
    
    TLSCoEventLoop->GetCurrentCoroutine()->SetState(Coroutine::HOLD);
    TLSCoEventLoop->GetCurrentCoroutine()->SwapTo(TLSCoEventLoop->GetMainCoroutine());
    
    //if(timer) {
        //定时器会被延时删除
        //timer->Cancel();
   // }
    
    return  ::accept(fd, addr, len);
}

ssize_t hook_read(int fd, void *buf, size_t nbyte) {
    
    
    //fd与event的映射
    //std::cout << pthread_getspecific(TLSCoEventLoopKey) << std::endl;
    CoEventLoop* TLSCoEventLoop = (CoEventLoop*)pthread_getspecific(util::PthreadKeysSingleton::GetInstance()->TLSEventLoop);
    //std::cout << TLSCoEventLoop << std::endl;
    
    if(TLSCoEventLoop == 0) {
        
        return ::read(fd, buf, nbyte);
        
    }
    CoEvent* event = TLSCoEventLoop->GetEvent(fd);
    if(!event || event->Flags() & O_NONBLOCK) {
        return ::read(fd, buf, nbyte);
    }
    
    int timeout = 0;
    //Timer* timer = TLSCoEventLoop->AddTimer(timeout, [event](){
        //event->disable
       // event->Remove();
        //close connection
   // });
    //避免重复添加事件
    if(!event->Readable()) event->EnableReading();
    
    TLSCoEventLoop->GetCurrentCoroutine()->SetState(Coroutine::HOLD);
    TLSCoEventLoop->GetCurrentCoroutine()->SwapTo(TLSCoEventLoop->GetMainCoroutine());
    
    //if(timer) {
        //定时器会被延时删除
       // timer->Cancel();
    //}
    
    return ::read(fd, buf, nbyte);
}

ssize_t hook_write(int fd, const void *buf, size_t nbyte) {
    //fd与event的映射
    
    CoEventLoop* TLSCoEventLoop = (CoEventLoop*)pthread_getspecific(util::PthreadKeysSingleton::GetInstance()->TLSEventLoop);
    
    CoEvent* event = TLSCoEventLoop->GetEvent(fd);
//    if(!event || event->Flags() & O_NONBLOCK) {
//        ssize_t n = ::write(fd, buf, nbyte);
//        if(n > 0) return n;
//        else {
//            TLSCoEventLoop->GetCurrentCoroutine()->SetState(Coroutine::HOLD);
//            TLSCoEventLoop->GetCurrentCoroutine()->SwapTo(TLSCoEventLoop->GetMainCoroutine());
//        }
//    }
    
    int timeout = 0;
    //Timer* timer = TLSCoEventLoop->AddTimer(timeout, [event](){
        //event->disable
       // event->Remove();
        //close connection
    //});
    //避免重复添加事件
    if(!event->Writable()) event->EnableWriting();
    
    TLSCoEventLoop->GetCurrentCoroutine()->SetState(Coroutine::HOLD);
    TLSCoEventLoop->GetCurrentCoroutine()->SwapTo(TLSCoEventLoop->GetMainCoroutine());
    
    //if(timer) {
        //定时器会被延时删除
        //timer->Cancel();
    //}
    
    return ::write(fd, buf, nbyte);
}


}
}
}
