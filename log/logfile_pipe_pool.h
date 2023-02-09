#ifndef CWEB_LOG_LOGFILEPIPEPOOL_H_
#define CWEB_LOG_LOGFILEPIPEPOOL_H_

#include <vector>
#include <mutex>

#include "logfile_pipe.h"
#include "../util/singleton.h"

//需要动态扩容 不用无锁队列
using namespace cweb::util;

namespace cweb {

namespace log {

class LogfilePipePool {
public:
    LogfilePipePool();
    void storePipe(LogfilePipePool* pipe);
    LogfilePipePool* fetchPipe();

private:
    std::vector<LogfilePipe *> logfilepipes_;
    std::mutex mutex_;

};

typedef Singleton<LogfilePipePool> LogfilePipePoolSingleton;

}

}

#endif