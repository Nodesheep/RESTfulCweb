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
    std::vector<T*> container_;
    Compare compare_;
    //移除时下渗
    void downturn(size_t n) {
        size_t child = n * 2 + 1;
        while(child < Size()) {
            if(child + 1 < Size() && compare_(*container_[child], *container_[child + 1])) {
                child++;
            }
            
            if(compare_(*container_[n], *container_[child])) {
                std::swap(container_[n], container_[child]);
                n = child;
                child = n * 2 + 1;
            }else {
                break;
            }

        
        }
    }
    //插入时上渗
    void upturn(size_t n) {
        //找到父节点
        size_t parent = (n - 1) / 2;
        while(n) {
            if(compare_(*container_[parent], *container_[n])) {
                std::swap(container_[parent], container_[n]);
                //T temp = _container[parent];
                //_container[parent] = _container[n];
                //_container[n] = temp; //swap底层会调用复制构造函数和=运算符，所以对应类必须进行显示实现！！！
                n = parent;
                parent = (n - 1) / 2;
            }else {
                break;
            }
        }
    }
};

}
}

#endif
