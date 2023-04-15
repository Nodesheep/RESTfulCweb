#ifndef CWEB_LOG_LOGFILEPIPE_H_
#define CWEB_LOG_LOGFILEPIPE_H_

#include <memory>
#include "log_info.h"
#include "lockfree_queue.h"

using namespace cweb::util;

namespace cweb {

namespace log {

class LogfilePipe {
private:
    LockfreeQueue<LogInfo *> logs_;

public:
    LogfilePipe(int capacity);
    
    bool MultiplePush(LogInfo *log);
    bool SinglePush(LogInfo *log);
    
    LogInfo* MultiplePop();
    LogInfo* SinglePop();

};

}
}

#endif
