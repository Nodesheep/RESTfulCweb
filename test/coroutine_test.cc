#include <iostream>
#include <string>
#include "coroutine_context.h"
#include "coroutine.h"

using namespace cweb::tcpserver::coroutine;

void f1();
void f2();


Coroutine c1([](){
    f1();
});

Coroutine c2([](){
    f2();
});

int f3(int a, int b) {
    return a + b;
}

void f1() {
   // while(1) {
        int a = 0;
        int b = 6;
        std::cout << a + b << std::endl;
        for(int i = 0;i < 5; i++){
            std::cout << "我是f1" << f3(i, i*i) << std::endl;
            c1.SwapTo(&c2);
        }
    //}
}

void f2() {
    
   //while(1) {
        std::string s("hello my world");
        for(int i = 0; i < s.size(); ++i) {
            std::cout << s[i] << std::endl;
        }
        
        for(int i = 0;i < 5; i++){
            std::cout << "我是f2" << std::endl;
            //CoroutineContext::ContextSwap(&c2, &c1);
            c2.SwapTo(&c1);
        }
    //}
}

//int main(int argc, const char * argv[]) {
//    // insert code here...
//    //std::cout << "hello" << std::endl;
//    f1();
//
//    return 0;
//}
