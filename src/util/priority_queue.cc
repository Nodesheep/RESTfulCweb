#include "priority_queue.h"

namespace cweb {
namespace util {

template<class T, class Compare>
void PriorityQueue<T, Compare>::downturn(size_t n) {
    size_t child = n * 2 + 1;
    while(child < Size()) {
        if(child + 1 < Size() && compare(*container_[child], *container_[child + 1])) {
            child++;
        }
        
        if(compare(*container_[n], *container_[child])) {
            std::swap(container_[n], container_[child]);
            n = child;
            child = n * 2 + 1;
        }else {
            break;
        }

    
    }
}

template<class T, class Compare>
void PriorityQueue<T, Compare>::upturn(size_t n) {
    //找到父节点
    size_t parent = (n - 1) / 2;
    while(n) {
        if(compare(*container_[parent], *container_[n])) {
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

}
}
