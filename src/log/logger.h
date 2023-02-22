#ifndef CWEB_LOG_LOGGER_H_
#define CWEB_LOG_LOGGER_H_

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

#include "log_appender.h"
#include "log_info.h"
#include "log_formatter.h"
#include "../cweb_config.h"
#include "../util/singleton.h"

namespace cweb {
        
namespace log {
   
class Logger {
public:
    typedef std::shared_ptr<Logger> shared_ptr;

    Logger(const std::string& name = "root");

    void Log(LogLevel level, LogInfo* logInfo);

    void Debug(LogInfo*  logInfo);
    void Info(LogInfo*  logInfo);
    void Warn(LogInfo*  logInfo);
    void Error(LogInfo*  logInfo);
    void Fatal(LogInfo*  logInfo);

    void AddAppender(LogAppender* appender);
    //void DelAppender(LogAppender* appender);

private:
    std::vector<LogAppender*> appenders_;
    LogLevel log_level_;

};

//单例
class LoggerManager {
public:
    //formatter共享 loop共享
    Logger::shared_ptr GetLogger(const std::string& key); //创建

private:
    std::unordered_map<std::string, Logger::shared_ptr> loggers_;
    std::mutex mutex_;
};

typedef cweb::util::Singleton<LoggerManager> LoggerManagerSingleton;
}

}

#endif
