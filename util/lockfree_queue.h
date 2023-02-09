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
    std::atomic<int> front_ = 0;
    std::atomic<int> back_ = 0;
    std::atomic<int> write_ = 0;
    int capacity_;

public:
    //能不能存智能指针？
    LockfreeQueue(int capacity);
    bool multiplePush(const T& val);
    bool multiplePop(T& val);
    bool singlePush(const T& val);
    bool singlePop(T& val);

};

}
}

#endif