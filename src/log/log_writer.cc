#include "log_writer.h"

namespace cweb {
namespace log {

LogWriter::LogWriter(int capacity) {
    size_t size = sizeof(LogInfo);
    logfilepipe_ = new LogfilePipe(capacity);
    for(int i = 0; i < capacity; ++i) {
        logfilepipe_->SinglePush((LogInfo*)(memorypool_->Allocate(size)));
    }
}

void LogWriter::AddTask(Functor cb) {
    std::unique_lock<std::mutex> lock(mutex_);
    tasks_.push_back(std::move(cb));
}

void LogWriter::Run() {
    running_ = true;
    loop();
}

void LogWriter::Stop() {
    running_ = false;
}

void LogWriter::Wakeup() {
    cond_.notify_all();
}

void LogWriter::Sleep() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock);
}

LogInfo* LogWriter::AllocLogInfo() {
    LogInfo* info = logfilepipe_->MultiplePop();
    while(!info) {
        Wakeup();
        info = logfilepipe_->MultiplePop();
    }
    return info;
}

void LogWriter::DeallocLogInfo(LogInfo *info) {
    logfilepipe_->SinglePush(info);
}

void LogWriter::loop() {
    while(running_) {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait_for(lock, std::chrono::seconds(60));
        for(auto task : tasks_) {
            task();
        }
    }
}


}
}
