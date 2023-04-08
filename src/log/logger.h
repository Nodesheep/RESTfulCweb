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
#include "log_writer.h"
//#include "cweb_config.h"
#include "singleton.h"

namespace cweb {
namespace log {
   
class Logger {
public:

    Logger(const std::string& module = "root");
    ~Logger();

    void Log(LogLevel level, const std::string& module, const std::string& tag, const char *format, ...);

    void AddAppender(LogAppender* appender);
    //void DelAppender(LogAppender* appender);

private:
    const std::string module_;
    std::vector<LogAppender*> appenders_;
    LogLevel log_level_ = LOGLEVEL_INFO;
    
    void log(LogLevel level, LogInfo* loginfo);

};

//单例
class LoggerManager {
public:
    //formatter共享 loop共享
    LoggerManager();
    ~LoggerManager();
    Logger* GetLogger(const std::string& module); //创建
    void log();

private:
    LogFormatter* formatter_;
    LogWriter* writer_;
    std::unordered_map<std::string, Logger*> loggers_;
    std::mutex mutex_;
    std::thread writer_thread_;
};

typedef cweb::util::Singleton<LoggerManager> LoggerManagerSingleton;

#define CWEB_MODULE "CWEB"

#define LOG(level, module, tag, format, ...) \
    LoggerManagerSingleton::GetInstance()->GetLogger(module)->Log(level, module, tag, format, ##__VA_ARGS__)
    
}
}

#endif
