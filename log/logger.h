#ifndef CWEB_LOG_LOGGER_H_
#define CWEB_LOG_LOGGER_H_

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

#include "log_appender.h"
#include "log_info.h"
#include "../cweb_config.h"

namespace cweb {
        
namespace log {
   
class Logger {
public:
    typedef std::shared_ptr<Logger> shared_ptr;

    Logger(const std::string& name = "root");

    void log(LogLevel level, LogInfo* logInfo);

    void debug(LogInfo*  logInfo);
    void info(LogInfo*  logInfo);
    void warn(LogInfo*  logInfo);
    void error(LogInfo*  logInfo);
    void fatal(LogInfo*  logInfo);

    void addAppender(LogAppender::shared_ptr appender);
    void delAppender(LogAppender::shared_ptr appender);

private:
    std::vector<LogAppender::shared_ptr> appenders_;
    LogLevel log_level_;

};

//单例
class LoggerManager {
public:
    Logger::shared_ptr getLogger(const std::string& key); //创建

private:
    std::unordered_map<std::string, Logger::shared_ptr> loggers_;
    std::mutex mutex_;
};

}

}

#endif