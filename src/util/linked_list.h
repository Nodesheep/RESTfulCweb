#ifndef CWEB_UTIL_LINKEDLIST_H_
#define CWEB_UTIL_LINKEDLIST_H_

#include <stddef.h>
#include <assert.h>
#include <type_traits>

namespace cweb {
namespace util {

class LinkedListNode {
public:
    LinkedListNode* pre = nullptr;
    LinkedListNode* next = nullptr;
};

template <typename T>
class LinkedList {
    static_assert((std::is_base_of<LinkedListNode, T>::value), "T must inherit LinkedListNode");
private:
    T* head_ = nullptr;
    T* tail_ = nullptr;
    size_t size_;
    
    void push(T* begin, T* end, size_t size) {
        if(size_ == 0) {
            head_ = begin;
            tail_ = end;
            size_ = size;
        }else {
            tail_->next = begin;
            begin->pre = tail_;
            end->next = nullptr;
            tail_ = end;
            size_ += size;
        }
    }
    
public:
    LinkedList() : size_(0) {}
    
    T* Pop() {
        T* value = head_ ? head_ : nullptr;
        if(value == nullptr) return value;
        if(size_ == 1) {
            Clear();
        }else {
            head_ = (T*)(head_->next);
            head_->pre = tail_;
            tail_->next = head_;
            --size_;
        }
        return value;
    }
    
    void Push(T* val) {
        if(val == nullptr) return;
        
        if(size_ == 0) {
            head_ = tail_ = val;
            tail_->next = nullptr;
            head_->pre = nullptr;
        }else {
            tail_->next = val;
            val->pre = tail_;
            val->next = nullptr;
            tail_ = val;
        }
        ++size_;
    }
    
    void Push(LinkedList<T>& list) {
        push(list.Front(), list.Back(), list.Size());
        list.Clear();
    }
    
    T* Front() {
        return head_ ? head_ : nullptr;
    }
    
    T* Back() {
        return tail_ ? tail_ : nullptr;
    }
    
    void Clear() {
        head_ = tail_ = nullptr;
        size_ = 0;
    }
    
    void Erase(T* val) {
        assert(val != nullptr);
        if(size_ == 0) return;
        if(val->pre == nullptr) {
            head_ = (T*)head_->next;
            if(head_) head_->pre = nullptr;
        }else if(val->next == nullptr){
            tail_ = (T*)val->pre;
            if(tail_) tail_->next = nullptr;
        }else {
            val->pre->next = val->next;
            val->next->pre = val->pre;
        }
        --size_;
    }
    
    T* Next(T* current) {
        return current->next ? (T*)current->next : nullptr;
    }
    
    size_t Size() {return size_;}

};

}
}

#endif
