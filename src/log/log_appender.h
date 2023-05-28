#ifndef CWEB_LOG_LOGAPPENDER_H_
#define CWEB_LOG_LOGAPPENDER_H_

#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <fstream>
#include "log_writer.h"

namespace cweb {

namespace log {

struct LogInfo;
class LogfilePipe;
class LogFormatter;

class LogAppender {
protected:
    LogFormatter* formatter_;
    
public:
    LogAppender(LogFormatter* formatter) : formatter_(formatter) {}
    virtual ~LogAppender() {}
    virtual void Log(LogInfo* logInfo) = 0;

};

class ConsoleAppender : public LogAppender {
    
private:
    std::mutex mutex_;

public:
    ConsoleAppender(LogFormatter* formatter) : LogAppender(formatter) {}
    virtual ~ConsoleAppender() {}
    virtual void Log(LogInfo* logInfo) override;

};

class FileAppender : public LogAppender {

private:
    std::string filepath_ = "";
    //写入时是多写单读
    LogfilePipe* logging_pipe_;
    LogWriter* writer_;
    std::string module_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::ofstream ofs_;
    bool writing_ = false;

    bool createFilepath();
    void writeLogsfile();

public:
    FileAppender(LogFormatter* formatter, LogWriter* writer, const std::string& module, const std::string& filepath = "../logfile");
    virtual ~FileAppender();
    virtual void Log(LogInfo* logInfo) override;

};

class DailyFileAppender : public FileAppender {
    
};

class LimitSizeFileAppender : public FileAppender {

};

}
}

#endif
