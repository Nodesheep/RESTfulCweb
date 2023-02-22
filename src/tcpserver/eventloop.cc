#include "eventloop.h"

namespace cweb {
namespace tcpserver {

EventLoop::EventLoop() {}

EventLoop::~EventLoop() {}

void EventLoop::Run() {
    running_ = true;
    loop();
}

void EventLoop::Stop() {
    running_ = false;
}

void EventLoop::Quit() {
    
}

void EventLoop::loop() {
    
}


}
}
