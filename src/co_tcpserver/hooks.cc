#include "hooks.h"
#include "coroutine.h"
#include "timer.h"
#include "co_event.h"
#include "processer.h"
#include "event_manager.h"
#include "../util/singleton.h"
#include <fcntl.h>
#include <unistd.h>

namespace cweb {
namespace tcpserver {
namespace coroutine {

ssize_t hook_read(int fd, void *buf, size_t nbyte) {
    
    
    //fd与event的映射
    CoEvent* event = (CoEvent*)EventManagerSingleton::GetInstance()->GetEvent(fd);
    if(!event || event->Flags() & O_NONBLOCK) {
        return ::read(fd, buf, nbyte);
    }
    
    int timeout = 0;
    Timer* timer = TLSProceser->AddTimer(timeout, [event](){
        //event->disable
        event->Remove();
        //close connection
    });
    //避免重复添加事件
    if(!event->Readable()) event->EnableReading();
    
    TLSProceser->GetCurrentCoroutine()->SetState(Coroutine::HOLD);
    TLSProceser->GetCurrentCoroutine()->SwapOut();
    
    if(timer) {
        //定时器会被延时删除
        timer->Cancel();
    }
    
    return ::read(fd, buf, nbyte);
}

}
}
}
