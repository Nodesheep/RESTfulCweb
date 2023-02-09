#include "log_appender.h"
#include "logfile_pipe.h"

namespace cweb {

namespace log {

void FileAppender::log(LogInfo* logInfo) {
    if(!logging_pipe_->push(logInfo)) {
        std::unique_lock<std::mutex> lock(mutex_);
        if(!logging_pipe_->push(logInfo)) {
            full_pipes_.push_back(std::move(logging_pipe_));
            logging_pipe_ = nullptr; //获取新的
        }
    }
}

//单线程写文件
void FileAppender::writeLogsfile() {
    writing_pipes_.swap(full_pipes_);//非线程安全
    LogInfo *loginfo = nullptr;
    for(auto pipe : writing_pipes_) {
        while(1) {
            loginfo = pipe->pop();
            if(!loginfo) break;
            //format
            //写文件
            delete loginfo;
        }
        //pipe挪回仓库
    }
    writing_pipes_.clear();
}



}

}