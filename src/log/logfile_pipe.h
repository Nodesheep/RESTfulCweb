#ifndef CWEB_LOG_LOGFILEPIPE_H_
#define CWEB_LOG_LOGFILEPIPE_H_

#include <memory>
#include "log_info.h"
#include "../util/lockfree_queue.h"

using namespace cweb::util;

namespace cweb {

namespace log {

//class LockfreeQueue;
class LogfilePipe {
private:
    size_t element_size_;
    LockfreeQueue<LogInfo *> logs_;

public:
    LogfilePipe(int capacity);
    
    //验证数组中指针释放后会不会被销毁
    bool Push(LogInfo *log);
    LogInfo* Pop();

};

}
}

#endif
