#include "threadlocal_memorypool.h"
#include <iostream>

namespace cweb {

namespace util {

static const size_t kMemoryListsLength = 8;
static const size_t kMaxMemeorySize = 1024;
static const size_t kMemoryBlockSize = 4096;

void MemoryList::PushFront(void* obj) {
    //前4/8个字节存储下一节点的地址
    *(void**)obj = head_;
    head_ = obj;
    ++nodes_remaining_;
}

void* MemoryList::PopFront() {
    if(nodes_remaining_ == 0) {
        return nullptr;
    }
    
    void* head = head_;
    head_ = *(void**)head_;
    --nodes_remaining_;
    return head;
}

bool MemoryList::Empty() {
    return nodes_remaining_ == 0;
}

MemoryPool::MemoryPool() : memorylists_(std::vector<MemoryList>(kMemoryListsLength)) {}

MemoryPool::~MemoryPool() {
    for(size_t i = 0; i < memoryblocks_.size(); ++i) {
        delete[] memoryblocks_[i];
    }
}

void* MemoryPool::Allocate(size_t bytes) {
    if(bytes <= kMaxMemeorySize) {
        size_t index = memorylistsIndex(bytes);
        
        if(memorylists_[index].Empty()) {
            return allocateInMemoryblocks(bytes);
        }else {
            return memorylists_[index].PopFront();
        }
    }else {
        return malloc(bytes);
    }
}

void MemoryPool::Deallocate(void* ptr, size_t bytes) {
    if(bytes <= kMaxMemeorySize) {
        size_t index = memorylistsIndex(bytes);
        memorylists_[index].PushFront(ptr);
    }else {
        free(ptr);
    }
}

size_t MemoryPool::memorylistsIndex(size_t bytes) {
    size_t index = 0;
    size_t bytes_canuse = 8;
    while(bytes_canuse < bytes) {
        ++index;
        bytes_canuse *= 2;
    }
    return index;
}

void* MemoryPool::allocateInMemoryblocks (size_t bytes) {
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
        alloc_ptr_ = new char[kMemoryBlockSize];
        memoryblocks_.push_back(alloc_ptr_);
        bytes_remaining_ = kMemoryBlockSize;
        allocptr = alloc_ptr_;
        alloc_ptr_ += bytes;
        bytes_remaining_ -= bytes;
    }
    return allocptr;
}

}

}
