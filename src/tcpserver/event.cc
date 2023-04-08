#include "event.h"
#include "eventloop.h"
#include "timer.h"

namespace cweb {
namespace tcpserver {

void Event::EnableReading() {
    events_ |= READ_EVENT;
    update();
}

void Event::EnableWriting() {
    events_ |= WRITE_EVENT;
    update();
}

void Event::DisableReading() {
    events_ &= ~READ_EVENT;
    update();
}

void Event::DisableWriting() {
    events_ &= ~WRITE_EVENT;
    update();
}

void Event::DisableAll() {
    events_ = 0;
    update();
}

void Event::HandleEvent(Time receiveTime) {
    if(revents_ & READ_EVENT) {
        read_callback_(receiveTime);
    }
    
    if(revents_ & WRITE_EVENT) {
        write_callback_();
    }
}

void Event::Remove() {
    loop_->RemoveEvent(this);
}

void Event::update() {
    loop_->UpdateEvent(this);
}

}
}
