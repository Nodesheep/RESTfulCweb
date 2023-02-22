#include "logger.h"


namespace cweb {

namespace log {

Logger::Logger(const std::string& name) {
    
}

void Logger::Log(LogLevel level, LogInfo* logInfo) {
    if(level >= log_level_) {
        for(auto& appender : appenders_) {
            appender->log(logInfo);
        }
    }
}

void Logger::Debug(LogInfo *logInfo) {
    Log(LOGLEVEL_DEBUG, logInfo);
}

void Logger::Info(LogInfo *logInfo) {
    Log(LOGLEVEL_INFO, logInfo);
}

void Logger::Warn(LogInfo *logInfo) {
    Log(LOGLEVEL_WARN, logInfo);
}

void Logger::Error(LogInfo *logInfo) {
    Log(LOGLEVEL_ERROR, logInfo);
}

void Logger::Fatal(LogInfo *logInfo) {
    Log(LOGLEVEL_FATAL, logInfo);
}


void Logger::AddAppender(LogAppender* appender) {
    appenders_.push_back(appender);
}

Logger::shared_ptr LoggerManager::GetLogger(const std::string& name) {
    if(loggers_.find(name) == loggers_.end()) {
        //多线程
        std::unique_lock<std::mutex> lock(mutex_);
        Logger::shared_ptr logger(new Logger());
        logger->AddAppender(new FileAppender());

    }
    return loggers_[name];
}
} 
}
