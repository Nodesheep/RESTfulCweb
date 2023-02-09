#include "logger.h"

namespace cweb {

namespace log {

void Logger::log(LogLevel level, LogInfo* logInfo) {
    if(level >= log_level_) {
        for(auto& appender : appenders_) {
            appender->log(logInfo);
        }
    }
}

void Logger::addAppender(LogAppender::shared_ptr appender) {
    appenders_.push_back(appender);
}

Logger::shared_ptr LoggerManager::getLogger(const std::string& name) {
    if(loggers_.find(name) == loggers_.end()) {
        //多线程
        std::unique_lock<std::mutex> lock(mutex_);
        Logger::shared_ptr logger(new Logger());
        logger->addAppender(new FileAppender());

    }
    return loggers_[name];
}
} 
}