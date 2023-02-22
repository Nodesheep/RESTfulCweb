#include "logfile_pipe.h"

namespace cweb {
namespace log {

LogfilePipe::LogfilePipe(int capacity) : logs_(LockfreeQueue<LogInfo*>(capacity)){}

bool LogfilePipe::Push(LogInfo *log) {
    return logs_.MultiplePop(log);
}

LogInfo* LogfilePipe::Pop() {
    LogInfo* info = nullptr;
    logs_.SinglePop(info);
    return info;
}
                         
}
}
