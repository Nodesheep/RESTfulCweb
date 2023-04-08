#include "log_writer.h"

namespace cweb {
namespace log {

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

void LogWriter::loop() {
    while(running_) {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait_for(lock, std::chrono::seconds(60));
        for(Functor task : tasks_) {
            task();
        }
    }
}


}
}
