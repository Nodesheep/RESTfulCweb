#include <iostream>
#include <string>
#include <thread>
#include "lockfree_queue.h"

using namespace cweb::util;

LockfreeQueue<int> queue1(1000);
LockfreeQueue<int> queue2(100010);

void func1() {
    for(int i = 0; i < 100000;) {
        if(!queue1.MultiplePush(i)) {
            std::cout << "failed:" << i << std::endl;
        }else {
            i += 4;
        }
    }
}

void func2() {
    for(int i = 1; i < 100000;) {
        if(!queue1.MultiplePush(i)) {
            std::cout << "failed:" << i << std::endl;
        }else {
            i += 4;
        }
    }
}

void func3() {
    for(int i = 2; i < 100000;) {
        if(!queue1.MultiplePush(i)) {
            std::cout << "failed:" << i << std::endl;
        }else {
            i += 4;
        }
    }
}

void func4() {
    for(int i = 3; i < 100000;) {
        if(!queue1.MultiplePush(i)) {
            std::cout << "failed:" << i << std::endl;
        }else {
            i += 4;
        }
    }
}


void func5() {
    int val = 0;
    int count = 200000;
    int ref = 0;
    while(count) {
        if(queue1.MultiplePop(val)) {
            ++ref;
            queue2.MultiplePush(val);
        }
        --count;
    }
    std::cout << "ref = " << ref << std::endl;
}


int main() {
    std::thread t1(func1);
    std::thread t2(func2);
    std::thread t3(func3);
    std::thread t4(func4);
    
    //sleep(3);
    std::thread t5(func5);
    std::thread t6(func5);
    std::thread t7(func5);
    std::thread t8(func5);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();

    int val = 0;
    int i = 0;
    std::vector<int> map(100000, 0);
    while(queue2.MultiplePop(val)) {
        ++i;
        map[val] = 1;
    }
    
    std::cout << "i = " << i << std::endl;
    
    for(int i = 0; i < 100000; ++i) {
        if(!map[i]) {
            std::cout << "lose:" << i << std::endl;
        }
    }
    return 0;
}
