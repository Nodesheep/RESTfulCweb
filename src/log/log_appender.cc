#include "log_appender.h"
#include "logfile_pipe.h"
#include "log_formatter.h"
#include "pthread_keys.h"
#include <sys/time.h>
#include <iostream>

namespace cweb {

namespace log {

static const size_t kMaxLogCapacity = 2000;

void ConsoleAppender::Log(LogInfo *logInfo) {
    std::unique_lock<std::mutex> lock(mutex_);
    std::cout << formatter_->Format(logInfo);
    
}

FileAppender::FileAppender(LogFormatter* formatter, LogWriter* writer, const std::string& module) : LogAppender(formatter), writer_(writer), module_(module) {
    logging_pipe_ = new LogfilePipe(kMaxLogCapacity);
    createFilepath();
    ofs_.open(filepath_, std::ofstream::out | std::ofstream::app);
    writer_->AddTask(std::bind(&FileAppender::writeLogsfile, this));
}

FileAppender::~FileAppender() {
    writeLogsfile();
    ofs_.close();
}

void FileAppender::Log(LogInfo* logInfo) {
    
    while(!logging_pipe_->MultiplePush(logInfo)) {
        writer_->Wakeup();
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock);
    }
    writer_->Wakeup();
}

bool FileAppender::createFilepath() {
    char timeStr[20];
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t seconds = tv.tv_sec;
    struct tm* tm = localtime(&seconds);
    strftime(timeStr, 20, "%Y-%m-%d", tm);
    std::string filepath = "../logfile/LOG_" + std::string(timeStr) + "_" + module_ + ".txt";

    if(filepath_.compare(filepath) != 0) {
        filepath_ = filepath;
        return true;
    }
    
    return false;
}

//单线程写文件
void FileAppender::writeLogsfile() {
    writing_ = true;
    if(createFilepath()) {
        ofs_.close();
        ofs_.open(filepath_, std::ofstream::out | std::ofstream::app);
    }
    
    if(!ofs_.is_open()) {
        ofs_.open(filepath_, std::ofstream::out | std::ofstream::app);
    }
    
    LogInfo *loginfo = logging_pipe_->SinglePop();
    while(loginfo) {
        ofs_ << formatter_->Format(loginfo);
        writer_->DeallocLogInfo(loginfo);
        loginfo = logging_pipe_->SinglePop();
        cond_.notify_all();
        ofs_.flush();
    }
    writing_ = false;
}



}

}
