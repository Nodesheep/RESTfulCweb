#ifndef CWEB_UTIL_THREADLOCALMEMORYPOOL_H_
#define CWEB_UTIL_THREADLOCALMEMORYPOOL_H_

#include <stddef.h>
#include <vector>
#include <pthread.h>

#include "noncopyable.h"

//小内存管理，小于1k
namespace cweb {

namespace util {

class MemoryList {

public:
    void PushFront(void* obj);
    void* PopFront();
    bool Empty();

private:
    void* head_ = nullptr;
    size_t size_ = 0;
};

class MemoryPool : public Noncopyable {

public:
    MemoryPool();
    void* Allocate(size_t bytes);
    void Deallocate(void* ptr, size_t bytes);

private:
    std::vector<MemoryList> memorylists_;
    char* alloc_ptr_;
    size_t bytes_remaining_;
    size_t real_used_;
    std::vector<char*> memoryblocks_;

    size_t MemorylistsIndex(size_t bytes);
    void* AllocateInMemoryblocks(size_t bytes);


};

static __thread MemoryPool* TLSMemoryPool;
//static std::__thread_local_data MemoryPool* TLSMemoryPool = new MemoryPool();

}

}


#endif