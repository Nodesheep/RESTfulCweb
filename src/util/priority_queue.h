#ifndef CWEB_UTIL_PRIORITYQUEUE_H_
#define CWEB_UTIL_PRIORITYQUEUE_H_

#include <vector>

namespace cweb {
namespace util {

template<class T>
struct less {
    bool operator()(const T& left, const T& right) {
        return left < right; //存的指针比较的就是指针
    }
};

template<class T>
struct greater {
    bool operator()(const T& left, const T& right) {
        return left > right;
    }
};

template<class T, class Compare = greater<T> >
class PriorityQueue {

public:
    
    void Push(T* const elem) {
        container_.push_back(elem);
        upturn(Size() - 1);
    }

    void Pop() {
        if(!Empty()) {
           // std::swap(<#_Tp &__x#>, <#_Tp &__y#>)
            std::swap(container_.front(), container_.back());
            container_.pop_back();
            downturn(0);
        }
    }

    size_t Size() const {
        return container_.size();
    }

    T* Front() const {
        return container_.front();
    }

    bool Empty() const {
        return container_.empty();
    }
    
private:
    //移除时下渗
    void downturn(size_t n);
    //插入时上渗
    void upturn(size_t n);
    
    std::vector<T*> container_;
    Compare compare_;
};

}
}

#endif
