#ifndef CWB_LOG_LOGINFO_H_
#define CWB_LOG_LOGINFO_H_

#include <memory>
#include <string>

namespace cweb {

namespace log {

enum LogLevel {
    LOGLEVEL_DEBUG,
    LOGLEVEL_INFO,
    LOGLEVEL_WARN,
    LOGLEVEL_ERROR,
    LOGLEVEL_FATAL,
    LOGLEVEL_OFF
};

struct LogInfo {
public:
    //%n 换行
    uint64_t time = 0; //%d
    LogLevel log_level; //%l
    unsigned long int thread_id; //&T
    std::string log_module; //%m
    std::string log_tag; //%t
    std::string log_content; //%c
};

}
}

#endif
