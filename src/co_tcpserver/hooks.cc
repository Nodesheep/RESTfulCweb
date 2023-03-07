#include "hooks.h"
#include "co_event.h"
#include "processer.h"
#include "event_manager.h"
#include "../util/singleton.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

ssize_t hook_read(int fd, void *buf, size_t nbyte) {
    
    
    //fd与event的映射
    CoEvent* event = (CoEvent*)EventManagerSingleton::GetInstance()->GetEvent(fd);
    //避免重复添加事件
    if(!event->Readable()) event->EnableReading();
    
    
    //if(!readable)
    
    return 0;
}

}
}
}
