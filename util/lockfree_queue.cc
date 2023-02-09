#include "lockfree_queue.h"

namespace cweb {

namespace util {

template <typename T>
LockfreeQueue<T>::LockfreeQueue(int capacity):capacity_(capacity), data_(std::vector<T>(capacity)){}

template <typename T>
bool LockfreeQueue<T>::multiplePush(const T& val) {

    int write, back;

    do {
        write = write_.load(std::memory_order_relaxed);
        if((write + 1) % capacity_ == front_.load(std::memory_order_acquire)) return false;
    }while(!write_.compare_exchange_strong(write, (write + 1) % capacity_), std::memory_order_relaxed);

    data_[write] =  val;

    do {
        back = write;
    }while(!back_.compare_exchange_strong(back, (back + 1) % capacity_), std::memory_order_release, std::memory_order_relaxed);

    return true;
}

template <typename T>
bool LockfreeQueue<T>::multiplePop(T& val) {
    int front;
    do {
        front = front_.load(std::memory_order_relaxed);
        if(front == back_.load(std::memory_order_acquire)) return false;
        val = data_[front];
    //pop中对front_的修改需要对push可见
    }while(!front_.compare_exchange_strong(front, (front + 1) % capacity_), std::memory_order_release, std::memory_order_relaxed);
    return true;
}

template <typename T>
bool LockfreeQueue<T>::singlePush(const T& val) {
    int back = back_.load(std::memory_order_relaxed);
    
    if((back + 1) % capacity_ == front_.load(std::memory_order_acquire)) return false;

    data_[back] = val;
    back_.store((back + 1) % capacity_, std::memory_order_release);
    return true;
}

template <typename T>
bool LockfreeQueue<T>::singlePop(T& val) {
    int front = front_.load(std::memory_order_relaxed);
    if(front == back_.load(std::memory_order_acquire)) return false;
    val = data_[front];
    front_.store((front + 1) % capacity_, std::memory_order_release);
    return true;
}
    
} 

}