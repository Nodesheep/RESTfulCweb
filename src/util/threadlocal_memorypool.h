#ifndef CWEB_UTIL_THREADLOCALMEMORYPOOL_H_
#define CWEB_UTIL_THREADLOCALMEMORYPOOL_H_

#include <stddef.h>
#include <vector>
#include <pthread.h>

#include "noncopyable.h"

//小内存管理，小于4k
namespace cweb {

namespace util {

class MemoryList {

public:
    void PushFront(void* obj);
    void* PopFront();
    bool Empty();

private:
    void* head_ = nullptr;
    size_t nodes_remaining_ = 0;
};

class MemoryPool : public Noncopyable {

public:
    MemoryPool();
    ~MemoryPool();
    
    void* Allocate(size_t bytes);
    void Deallocate(void* ptr, size_t bytes);

private:
    std::vector<MemoryList> memorylists_;
    char* alloc_ptr_ = nullptr;
    size_t bytes_remaining_ = 0;
    size_t real_used_ = 0;
    std::vector<char*> memoryblocks_;

    size_t memorylistsIndex(size_t bytes);
    void* allocateInMemoryblocks(size_t bytes);
};

}
}


#endif
