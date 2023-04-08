#include "timer.h"
#include <sys/time.h>
#include <math.h>

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
  executionTime_(Time::Now() += interval * 1000 * 1000) {}

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

//小顶堆实现定时器管理，无法实现定时器的实时销毁，只能等到定时器时间到时再销毁
TimerManager::TimerManager() : timers_(util::PriorityQueue<Timer>()) {}
TimerManager::~TimerManager() {
    
}

void TimerManager::AddTimer(Timer *timer) {
    std::unique_lock<std::mutex> lock(mutex_);
    timers_.Push(timer);
}

bool TimerManager::PopOneTimeoutTimer(Timer*& timer) {
    std::unique_lock<std::mutex> lock(mutex_);
    if(timers_.Size() == 0 || timers_.Front()->ExecutionTime() > Time::Now()) {
        return false;
    }
    timer = timers_.Front();
    timers_.Pop();
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

int TimerManager::NextTimeoutInterval() {
    return timers_.Size() ? (int)(timers_.Front()->ExecutionTime().MicroSecondsSinceEpoch() - Time::Now().MicroSecondsSinceEpoch()) : -1;
}


//时间轮实现定时器管理
TimerWheelManager::TimerWheelManager(uint64_t tickms, int wheelsize, int layers) : tickms_(tickms), wheel_size_(wheelsize), layers_(layers), last_rotate_time_(Time::Now()) {
    timers_wheels_ = std::vector<std::vector<util::LinkedList<Timer>>>(layers, std::vector<util::LinkedList<Timer>>(wheelsize));
    steps_ = std::vector<int>(layers, -1);
    timers_cnts_ = std::vector<int>(layers, 0);
}

TimerWheelManager::~TimerWheelManager() {
    
}

void TimerWheelManager::AddTimer(Timer *timer) {
    std::unique_lock<std::mutex> lock(mutex_);
    addTimer(timer);
}

void TimerWheelManager::addTimer(Timer *timer) {
    int interval = timer->executionTime_ < last_rotate_time_ ? 0 : (int)((timer->executionTime_ - last_rotate_time_) / 1000);
    for(int i = 0; i < layers_; ++i) {
        if(interval < tickms_ * pow(wheel_size_, i + 1)) {
            int step = (steps_[i] + (int)(interval / (tickms_ * pow(wheel_size_, i)))) % wheel_size_;
            timer->setPosition(i, step);
            timer->poped_ = false;
            util::LinkedList<Timer>& timers = timers_wheels_[i][step];
            timers.Push(timer);
            ++timers_cnts_[i];
            break;
        }
    }
}


bool TimerWheelManager::PopAllTimeoutFunctor(std::vector<std::function<void ()>> &funcs) {
    return false;
}

bool TimerWheelManager::PopAllTimeoutTimer(std::vector<Timer *> &timeout_timers) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    Time now = Time::Now();
    uint64_t interval = (now - last_rotate_time_) / 1000;
    last_rotate_time_ = now;
    int step = (int)(interval / tickms_);
    std::vector<int> steps(layers_,0);
    for(int i = 0; i < layers_; ++i) {
        steps[i] =  step - step / wheel_size_ * wheel_size_;
        step /= wheel_size_;
    }
    
    for(int i = 0;i < layers_; ++i) {
        steps[i] += steps_[i];
        if(i < layers_ - 1) {
            if(steps[i] > wheel_size_ - 1) {
                ++steps[i+1];
            }
        }
        steps[i] %= wheel_size_;
    }
    
    for(int i = layers_ - 1; i > 0; --i) {
        int end = steps[i] < steps_[i] ? steps[i] + wheel_size_ : steps[i];
        
        int j = steps_[i] < 0 ? steps_[i] + 1 : steps_[i];
        for(; j <= end; ++j) {
            util::LinkedList<Timer>& timers = timers_wheels_[i][j % wheel_size_];
            size_t size = timers.Size();
            while(size) {
               // std::cout << "请求链路：移除定时器：" << std::endl;
                Timer* timer = timers.Pop();
                timer->poped_ = true;
                --size;
                timers_cnts_[i]--;
                addTimer(timer);
            }
        }
    }
    
    //把第一层的所在格的所有定时任务取出
    bool res = false;
    
    int end = steps[0] < steps_[0] ? steps[0] + wheel_size_ : steps[0];
    int i = steps_[0] < 0 ? steps_[0] + 1 : steps_[0];
    for(; i <= end; ++i) {
        util::LinkedList<Timer>& timers = timers_wheels_[0][i % wheel_size_];
        size_t size = timers.Size();
        while(size) {
            //std::cout << "请求链路：移除定时器：" << std::endl;
            Timer* timer = timers.Pop();
            timer->poped_ = true;
            --size;
            timers_cnts_[0]--;
            if(!timer->cancel_) {
                timeout_timers.push_back(timer);
                res = true;
            }
            
            if(timer->PretendExecute()) {
                addTimer(timer);
            }
        }
    }
    
    steps_ = std::move(steps);
    return res;
}

int TimerWheelManager::NextTimeoutInterval() {
    for(int i = 0; i < layers_; ++i) {
        if(timers_cnts_[i] > 0) {
            return tickms_ * pow(wheel_size_, i);
        }
    }
    return -1;
}

void TimerWheelManager::RemoveTimer(Timer *timer) {
    if(timer == nullptr) return;
    std::unique_lock<std::mutex> lock(mutex_);
    if(!timer->poped_) {
        util::LinkedList<Timer>& timers = timers_wheels_[timer->position_[0]][timer->position_[1]];
        --timers_cnts_[timer->position_[0]];
        timers.Erase(timer);
        timer->poped_ = true;
    }
    delete timer;
}

}
}
