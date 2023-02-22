#ifndef CWEB_UTIL_LINKEDLIST_H_
#define CWEB_UTIL_LINKEDLIST_H_

#include <stddef.h>
#include <assert.h>

namespace cweb {
namespace util {

template <typename T>
class LinkedList {
private:
    struct Node {
        T* value;
        Node* next;
        Node* pre;
    };
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    size_t size_;
    
public:
    LinkedList() : size_(0) {}
    
    void FrontLockFree(T*& val) {
        val = (T*)head_;
    }
    
    void PushBackLockFree(T*& val) {
        if(val == nullptr) return;
        Node* newnode = new Node;
        newnode->value = val;
        
        if(size_ == 0) {
            head_ = newnode;
            tail_ = newnode;
            head_->next = tail_;
            head_->pre = tail_;
            tail_->next = head_;
            tail_->pre = head_;
        }else {
            newnode->next = tail_->next;
            tail_->next  = newnode;
            newnode->pre = tail_;
            tail_ = newnode;
            head_->pre = tail_;
        }
        ++size_;
    }
    
    void PopFrontLock(T*& val);
    void PushBackLock(T*& val);
    
    void Erase(T* val) {
        assert(val != nullptr);
        Node* node = (Node *)val;
        assert(node->pre != nullptr && node->next != nullptr);
        node->pre->next = node->next;
        node->next->pre = node->pre;
    }
    
    void Next(T* current, T*& next) {
    
    }

};

}
}

#endif
