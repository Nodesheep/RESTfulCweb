#ifndef CWB_LOG_LOGINFO_H_
#define CWB_LOG_LOGINFO_H_

#include <memory>

namespace cweb {

namespace log {

enum LogLevel {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        OFF
};

class LogInfo {
public:
    typedef std::shared_ptr<LogInfo> shared_ptr;
};

}
}

#endif