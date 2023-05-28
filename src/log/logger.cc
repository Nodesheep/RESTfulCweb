#include <sys/time.h>
#include "logger.h"
#include "pthread_keys.h"

namespace cweb {

namespace log {

static const size_t kMaxLogContentLength = 256;

Logger::Logger(const std::string& module, LogWriter* writer, LogLevel loglevel) : module_(module), writer_(writer), log_level_(loglevel) {}

Logger::~Logger() {
    for(LogAppender* appender : appenders_) {
        delete appender;
    }
}

void Logger::Log(LogLevel level, const std::string &module, const std::string &tag, const char *format, ...) {
    if(level >= log_level_) {
        LogInfo* info = writer_->AllocLogInfo();
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int64_t seconds = tv.tv_sec;
        info->log_level = level;
        info->time = seconds * 1000 * 1000 + tv.tv_usec;
        info->thread_id = (unsigned long int)pthread_self();
        info->log_module = module;
        info->log_tag = tag;
        char content[kMaxLogContentLength];
        va_list valst;
        va_start(valst, format);
        int n = vsnprintf(content, kMaxLogContentLength, format, valst);
        content[n] = '\0';
        info->log_content = std::string(content);
        va_end(valst);
        
        log(level, info);
    }
}

void Logger::log(LogLevel level, LogInfo* loginfo) {
    for(auto& appender : appenders_) {
        appender->Log(loginfo);
    }
}

void Logger::AddAppender(LogAppender* appender) {
    appenders_.push_back(appender);
}

LoggerManager::LoggerManager() {
    formatter_ = new LogFormatter(config_.log_pattern);
    writer_ = new LogWriter(config_.writer_capcity);
    writer_thread_ = std::thread([this](){
        writer_->Run();
    });
}

LoggerManager::~LoggerManager() {
    writer_->Stop();
    for(std::unordered_map<std::string, Logger*>::iterator iter = loggers_.begin(); iter != loggers_.end(); ++iter) {
        delete iter->second;
    }
    delete formatter_;
}

Logger* LoggerManager::GetLogger(const std::string& module) {
    auto iter = loggers_.find(module);
    if(iter == loggers_.end()) {
        Logger* logger = new Logger(module, writer_, config_.log_level);
        if(config_.need_console) {
            logger->AddAppender(new ConsoleAppender(formatter_));
        }
        logger->AddAppender(new FileAppender(formatter_, writer_, module, config_.log_filepath));
        std::unique_lock<std::mutex> lock(mutex_);
        loggers_[module] = logger;
        return logger;
    }
    return iter->second;
}

} 
}
