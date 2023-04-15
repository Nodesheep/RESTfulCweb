#include "logfile_pipe.h"

namespace cweb {
namespace log {

LogfilePipe::LogfilePipe(int capacity) : logs_(LockfreeQueue<LogInfo*>(capacity)){
}

bool LogfilePipe::MultiplePush(LogInfo *log) {
    return logs_.MultiplePush(log);
}

bool LogfilePipe::SinglePush(LogInfo *log) {
    return logs_.SinglePush(log);
}

LogInfo* LogfilePipe::MultiplePop() {
    LogInfo* info = nullptr;
    if(logs_.MultiplePop(info)) {
        return info;
    }else {
        return nullptr;
    }
}

LogInfo* LogfilePipe::SinglePop() {
    LogInfo* info = nullptr;
    if(logs_.SinglePop(info)) {
        return info;
    }else {
        return nullptr;
    }
    return info;
}
                         
}
}
