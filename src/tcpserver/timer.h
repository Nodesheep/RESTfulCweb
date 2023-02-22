#ifndef CWEB_TCP_TIMER_H_
#define CWEB_TCP_TIMER_H_

#include <string>
#include <memory>

#include "../util/priority_queue.h"

namespace cweb {
namespace tcpserver {

class Time {
private:
    uint64_t microseconds_since_epoch_;
    
public:
    inline int64_t MicroSecondsSinceEpoch() const{
        return microseconds_since_epoch_;
    }
    
    inline Time& operator += (int64_t interval) {
        microseconds_since_epoch_ += interval;
        return *this;
    }
    
    inline Time& operator -= (int64_t interval) {
        microseconds_since_epoch_ -= interval;
        return *this;
    }
    
    friend bool operator < (const Time& lt, const Time& rt) {
        return lt.MicroSecondsSinceEpoch() < rt.MicroSecondsSinceEpoch();
    }
    
    friend bool operator > (const Time& lt, const Time& rt) {
        return lt.MicroSecondsSinceEpoch() > rt.MicroSecondsSinceEpoch();
    }
    
    std::string ToString(const std::string& fmt = "%Y-%m-%d %H:%M:%S");
    static Time Now();
};

class Timer {
private:
    bool cancel_ = false;
    Time executionTime_;
    uint64_t interval_;
    int repeats_;
    std::function<void()> timer_callback_;
    
public:
    Timer(){}
    Timer(uint64_t interval, std::function<void()> cb, int repeats = 1);
    bool Execute();
    void Cancel() {cancel_ = true;}
    
    friend bool operator < (const Timer& lt, const Timer& rt) {
        return lt.executionTime_ < rt.executionTime_;
    }
    
    friend bool operator > (const Timer& lt, const Timer& rt) {
        return lt.executionTime_ > rt.executionTime_;
    }
    
    Timer& operator=(const Timer& task) {
        cancel_ = task.cancel_;
        executionTime_ = task.executionTime_;
        interval_ = task.interval_;
        repeats_ = task.repeats_;
        return *this;
    }
    
};

class TimerManager {
private:
    std::unique_ptr<util::PriorityQueue<Timer> > timers_;
    
public:
    void AddTimer(Timer* timer);
    //获取超时事件
    bool PopOneTimeoutTimer(Timer*& timer);
    bool PopAllTimeoutTimer(std::vector<Timer*>& timers);
    uint64_t NextTimeoutInterval();
};

}
}

#endif
