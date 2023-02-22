#ifndef CWEB_LOG_LOGAPPENDER_H_
#define CWEB_LOG_LOGAPPENDER_H_

#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace cweb {

namespace log {

class LogInfo;
class LogfilePipe;

class LogAppender {
public:

    virtual void log(LogInfo* logInfo) = 0;

};

class ConsoleAppender : public LogAppender {
    
private:
    std::mutex mutex_;

public:
    virtual void log(LogInfo* logInfo) override;

};

class FileAppender : public LogAppender {

private:
    std::string filepath_;
    LogfilePipe* logging_pipe_;
    std::vector<LogfilePipe* > full_pipes_;
    std::vector<LogfilePipe* > writing_pipes_;
    std::mutex mutex_;

    void writeLogsfile();

public:
    virtual void log(LogInfo* logInfo) override;

};

class DailyFileAppender : public FileAppender {
   // virtual void log(LogInfo::shared_ptr logInfo) override;
};

class LimitSizeFileAppender : public FileAppender {

};

}
}

#endif
