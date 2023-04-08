#include <iostream>
#include <string>
#include "linked_list.h"

using namespace cweb::util;

class Base : public LinkedListNode {
private:
    int index_;
public:
    Base(int index) : index_(index) {}
    void Say() {
        std::cout << "my index is " << index_ << std::endl;
    }
    
};

/*
int main() {
    LinkedList<Base> lists;
    
    
    lists.Push(new Base(2));
    lists.Push(new Base(3));
    lists.Push(new Base(4));
    lists.Push(new Base(5));
    lists.Push(new Base(6));
    lists.Push(new Base(7));
    
    Base* front = lists.Front();
    if(front) {
        front->Say();
        std::cout << "size: " << lists.Size() << std::endl;
    }
    int i = 100;
    while (front && i) {
        front = lists.Pop();
        lists.Push(front);
        if(front) front->Say();
        if(i % 5 == 0) {
            lists.Erase(front);
            std::cout << "size: " << lists.Size() << std::endl;
        }
        --i;
    }
    
    LinkedList<Base> lists1;
    
    
    lists1.Push(new Base(8));
    lists1.Push(new Base(9));
    lists1.Push(new Base(10));
    lists1.Push(new Base(11));
    lists1.Push(new Base(12));
    lists1.Push(new Base(13));
    
    std::cout << "lists1 size: " << lists1.Size() << std::endl;
    lists.Push(lists1);
    std::cout << "lists size: " << lists.Size() << std::endl;
    std::cout << "lists1 size: " << lists1.Size() << std::endl;
    
    i = 100;
    front = lists.Front();
    while (front && i) {
        front = lists.Pop();
        lists.Push(front);
        if(front) front->Say();
        if(i % 5 == 0) {
            lists.Erase(front);
            std::cout << "size: " << lists.Size() << std::endl;
        }
        --i;
    }
 
   lists1.Push(new Base(8));
   lists1.Push(new Base(9));
   lists1.Push(new Base(10));
   lists1.Push(new Base(11));
   lists1.Push(new Base(12));
   lists1.Push(new Base(13));
   front = lists1.Front();
   while(front = lists1.Next(front)) {
       front->Say();
   }
    
    return 0;
}
*/
