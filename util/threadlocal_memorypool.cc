#include "threadlocal_memorypool.h"

namespace cweb {

namespace util {

static const size_t kMemoryListsLength = 8;
static const size_t kMaxMemeorySize = 1024;
static const size_t kMemoryBlockSize = 4096;

MemoryPool::MemoryPool() : memorylists_(std::vector<MemoryList>(kMemoryListsLength)) {}

void* MemoryPool::Allocate(size_t bytes) {
    if(bytes <= kMaxMemeorySize) {
        size_t index = MemorylistsIndex(bytes);
        
        if(memorylists_[index].Empty()) {
            return AllocateInMemoryblocks(bytes);
        }else {
            return memorylists_[index].PopFront();
        }
        //去block中拿
    }else {
        
    }
}

void MemoryPool::Deallocate(void* ptr, size_t bytes) {
    
}

size_t MemoryPool::MemorylistsIndex(size_t bytes) {
    size_t index = 0;
    size_t bytes_canuse = 8;
    while(bytes_canuse < bytes) {
        ++index;
        bytes_canuse *= 2;
    }
    return index;
}

void* MemoryPool::AllocateInMemoryblocks (size_t bytes) {
    const int align = (sizeof(void*) > 8) ? sizeof(void*) : 8;
    size_t current_mod = reinterpret_cast<uintptr_t>(alloc_ptr_) & (align - 1);
    size_t slop = (current_mod == 0 ? 0 : align - current_mod);
    size_t bytesneed = bytes + slop;
    char* allocptr;
    if(bytesneed <= bytes_remaining_) {
        allocptr = alloc_ptr_ + slop;
        alloc_ptr_ += bytesneed;
        bytes_remaining_ -= bytesneed;
    }else {
        char* block = new char[kMemoryBlockSize];
        memoryblocks_.push_back(block);
        bytes_remaining_ = kMemoryBlockSize;
        allocptr = block;
        alloc_ptr_ += bytes;
        bytes_remaining_ -= bytes;
    }
    return allocptr;
}
}

}