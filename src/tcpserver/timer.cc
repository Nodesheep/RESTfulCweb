#include "timer.h"
#include <sys/time.h>

namespace cweb {
namespace tcpserver {

Time Time::Now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    
    return Time(seconds * 1000 * 1000 + tv.tv_usec);
}

std::string Time::ToString(const std::string& fmt) {
    char timeStr[128];
    time_t seconds = static_cast<time_t>(microseconds_since_epoch_ / (1000 * 1000));
    struct tm* tm = localtime(&seconds);
    strftime(timeStr, 128, fmt.data(), tm);
    return std::string(timeStr);
}

Timer::Timer(uint64_t interval, std::function<void()> cb, int repeats)
: interval_(interval),
  timer_callback_(std::move(cb)),
  repeats_(repeats),
  executionTime_(Time::Now() += interval * 100) {}

bool Timer::Execute() {
    if(cancel_) return false;
    
    if(timer_callback_) {
        timer_callback_();
    }
    
    if(--repeats_ == 0) return false;
    else {
        if(repeats_ < 0) ++repeats_;
        executionTime_ += interval_ * 1000;
        return true;
    }
}

bool Timer::PretendExecute() {
    if(cancel_ || --repeats_ == 0) return false;
    else {
        if(repeats_ < 0) ++repeats_;
        executionTime_ += interval_ * 1000;
        return true;
    }
}

void TimerManager::AddTimer(Timer *timer) {
    std::unique_lock<std::mutex> lock(mutex_);
    timers_->Push(timer);
}

bool TimerManager::PopOneTimeoutTimer(Timer*& timer) {
    std::unique_lock<std::mutex> lock(mutex_);
    if(timers_->Size() == 0 || timers_->Front()->ExecutionTime() > Time::Now()) {
        return false;
    }
    timer = timers_->Front();
    timers_->Pop();
    return true;
}

bool TimerManager::PopAllTimeoutTimer(std::vector<Timer *>& timers) {
    Timer* timer = nullptr;
    bool res = false;
    while(PopOneTimeoutTimer(timer)) {
        timers.push_back(timer);
        res = true;
    }
    return res;
}

bool TimerManager::PopAllTimeoutFunctor(std::vector<std::function<void ()>>& funcs) {
    Timer* timer = nullptr;
    bool res = false;
    while(PopOneTimeoutTimer(timer)) {
        res = true;
        if(!timer->cancel_) {
            funcs.push_back(std::move(timer->timer_callback_));
        }
        
        if(timer->PretendExecute()) {
            AddTimer(timer);
        }else {
            delete timer;
        }
    }
    return res;
}

void TimerManager::ExecuteAllTimeoutTimer() {
    timeout_timers_.clear();
    PopAllTimeoutTimer(timeout_timers_);
    for(Timer* timer : timeout_timers_) {
        if(timer->Execute()) {
            AddTimer(timer);
        }else {
            delete timer;
        }
    }
}

uint64_t TimerManager::NextTimeoutInterval() {
    return timers_->Size() ? timers_->Front()->ExecutionTime().MicroSecondsSinceEpoch() - Time::Now().MicroSecondsSinceEpoch() : (uint64_t)-1;
}

}
}
