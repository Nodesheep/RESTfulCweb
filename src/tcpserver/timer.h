#ifndef CWEB_TCP_TIMER_H_
#define CWEB_TCP_TIMER_H_

#include <string>
#include <memory>
#include <mutex>
#include <functional>

#include "priority_queue.h"
#include "linked_list.h"


//TODO 堆定时器导致cancel时timer无法被立即销毁，后续改用时间轮
namespace cweb {
namespace tcpserver {

class Time {
private:
    uint64_t microseconds_since_epoch_;
    
public:
    Time(int64_t microseconds) : microseconds_since_epoch_(microseconds) {}
    
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
    
    inline uint64_t operator - (const Time& time) {
        return microseconds_since_epoch_ - time.microseconds_since_epoch_;
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

class TimerManager;
class TimerWheelManager;
class Timer : public util::LinkedListNode {
private:
    bool cancel_ = false;
    Time executionTime_;
    uint64_t interval_;
    int repeats_;
    std::function<void()> timer_callback_;
    int position_[2];
    bool poped_ = false;
    
public:
    friend TimerManager;
    friend TimerWheelManager;
    //毫秒
    Timer(uint64_t interval, std::function<void()> cb, int repeats = 1);
    
    Time ExecutionTime() const {return executionTime_;}
    uint64_t ExecutionInterval() {return executionTime_ - Time::Now();}
    bool Execute();
    bool PretendExecute();
    void Cancel() {cancel_ = true;}
    
    void setPosition(int layers, int step) {
        position_[0] = layers;
        position_[1] = step;
    }
    
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

//小顶堆弃用
class TimerManager {
private:
    util::PriorityQueue<Timer> timers_;
    std::vector<Timer*> timeout_timers_;
    std::mutex mutex_;
    
public:
    TimerManager();
    virtual ~TimerManager();
    virtual void AddTimer(Timer* timer);
    //获取超时事件
    virtual void ExecuteAllTimeoutTimer();
    virtual bool PopOneTimeoutTimer(Timer*& timer);
    virtual bool PopAllTimeoutTimer(std::vector<Timer*>& timers);
    virtual bool PopAllTimeoutFunctor(std::vector<std::function<void()>>& funcs);
    virtual int NextTimeoutInterval();
    virtual void RemoveTimer(Timer* timer) = 0;
};

class TimerWheelManager : public TimerManager {
private:
    uint64_t tickms_;
    int wheel_size_;
    int layers_;
    std::vector<int> steps_;
    std::vector<int> timers_cnts_;
    std::vector<std::vector<util::LinkedList<Timer>>> timers_wheels_;
    std::mutex mutex_;
    Time last_rotate_time_;
    void addTimer(Timer* timer);
    
public:
    TimerWheelManager(uint64_t tickms = 1, int wheelsize = 100, int layers = 3);
    virtual ~TimerWheelManager();
    
    virtual void AddTimer(Timer* timer) override;
    virtual bool PopAllTimeoutFunctor(std::vector<std::function<void()>>& funcs) override;
    virtual bool PopAllTimeoutTimer(std::vector<Timer*>& timers) override;
    virtual int NextTimeoutInterval() override;
    virtual void RemoveTimer(Timer* timer) override;
};

}
}

#endif
