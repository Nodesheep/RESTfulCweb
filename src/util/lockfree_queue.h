#ifndef CWEB_UTIL_LOCKFREEQUEUE_H_
#define CWEB_UTIL_LOCKFREEQUEUE_H_

#include <vector>
#include <atomic>

namespace cweb {

namespace util {

template <typename T>
class LockfreeQueue {

private:
    std::vector<T> data_;
    std::atomic<int> front_ = {0};
    std::atomic<int> back_ = {0};
    std::atomic<int> write_ = {0};
    int capacity_;
    
public:
    LockfreeQueue<T>(int capacity = 100) : capacity_(capacity), data_(std::vector<T>(capacity)){}
    LockfreeQueue<T>(const LockfreeQueue<T>& queue) {
        capacity_ = queue.capacity_;
        data_ = queue.data_;
    }
    
    bool MultiplePush(const T val) {
        int write, back;

        do {
            write = write_.load(std::memory_order_relaxed);
            if((write + 1) % capacity_ == front_.load(std::memory_order_acquire)) return false;
        }while(!write_.compare_exchange_strong(write, (write + 1) % capacity_, std::memory_order_relaxed));

        data_[write] =  val;

        do {
            back = write;
        }while(!back_.compare_exchange_strong(back, (back + 1) % capacity_ , std::memory_order_release));

        return true;
    }
    
    bool MultiplePop(T& val) {
        int front;
        do {
            front = front_.load(std::memory_order_relaxed);
            if(front == back_.load(std::memory_order_acquire)) return false;
            val = data_[front];
        }while(!front_.compare_exchange_strong(front, (front + 1) % capacity_, std::memory_order_release));
        
        return true;
    }
    
    bool SinglePush(const T val) {
        int back = back_.load(std::memory_order_relaxed);
        
        if((back + 1) % capacity_ == front_.load(std::memory_order_acquire)) return false;

        data_[back] = val;
        back_.store((back + 1) % capacity_, std::memory_order_release);
        return true;
    }
    
    bool SinglePop(T& val) {
        int front = front_.load(std::memory_order_relaxed);
        if(front == back_.load(std::memory_order_acquire)) return false;
        val = data_[front];
        front_.store((front + 1) % capacity_, std::memory_order_release);
        return true;
    }

};

}
}

#endif
