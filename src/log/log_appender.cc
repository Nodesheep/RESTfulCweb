#include "log_appender.h"
#include "logfile_pipe.h"
#include "log_formatter.h"
#include "threadlocal_memorypool.h"
#include "pthread_keys.h"
#include <sys/time.h>
#include <iostream>

namespace cweb {

namespace log {

static const size_t kMaxLogCapacity = 100;

void ConsoleAppender::Log(LogInfo *logInfo) {
    std::unique_lock<std::mutex> lock(mutex_);
    std::cout << formatter_->Format(logInfo);
    
}

FileAppender::FileAppender(LogFormatter* formatter, LogWriter* writer, const std::string& module) : LogAppender(formatter), writer_(writer), module_(module) {
    logging_pipe_ = new LogfilePipe(kMaxLogCapacity);
    retrieving_pipe_ = new LogfilePipe(kMaxLogCapacity);
    createFilepath();
    ofs_.open(filepath_, std::ofstream::out | std::ofstream::app);
    writer_->AddTask(std::bind(&FileAppender::writeLogsfile, this));
}

FileAppender::~FileAppender() {
    //把剩余的全部写完
    writeLogsfile();
    ofs_.close();
}

void FileAppender::Log(LogInfo* logInfo) {
    while(!logging_pipe_->MultiplePush(logInfo)) {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock);
    }
    
    writer_->Wakeup();
    //回收队列 多读 写一个取一个
    LogInfo *loginfo = retrieving_pipe_->MultiplePop();
    if(loginfo) {
        ((util::MemoryPool*)pthread_getspecific(util::PthreadKeysSingleton::GetInstance()->TLSMemoryPool))->Deallocate(loginfo, sizeof(LogInfo));
    }
    
}

bool FileAppender::createFilepath() {
    char timeStr[20];
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t seconds = tv.tv_sec;
    struct tm* tm = localtime(&seconds);
    strftime(timeStr, 20, "%Y-%m-%d", tm);
    std::string filepath = "logfile/LOG_" + std::string(timeStr) + "_" + module_ + ".txt";

    if(filepath_.compare(filepath) != 0) {
        filepath_ = filepath;
        return true;
    }
    
    return false;
}

//单线程写文件
void FileAppender::writeLogsfile() {
    LogInfo *loginfo = logging_pipe_->SinglePop();
    if(!loginfo) return;
    
    writing_ = true;
    if(createFilepath()) {
        ofs_.close();
        ofs_.open(filepath_, std::ofstream::out | std::ofstream::app);
    }
    
    if(!ofs_.is_open()) {
        ofs_.open(filepath_, std::ofstream::out | std::ofstream::app);
    }
    
    while(loginfo) {
        ofs_ << formatter_->Format(loginfo);
        //不能放在写文件线程中回收，要放到他产生的线程中回收
        retrieving_pipe_->SinglePush(loginfo);
        
        loginfo = logging_pipe_->MultiplePop();
        cond_.notify_all();
    }
    ofs_.flush();
    writing_ = false;
}



}

}
