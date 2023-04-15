#include "epoll_poller.h"
#include "event.h"
#include "timer.h"
#include <unistd.h>

namespace cweb {
namespace tcpserver {

EPollPoller::EPollPoller(EventLoop* loop) : Poller(loop), epollfd_(::epoll_create1(EPOLL_CLOEXEC)), epoll_events_(1024) {}

EPollPoller::~EPollPoller() {
    ::close(epollfd_);
}

void EPollPoller::UpdateEvent(Event* event) {
    if(event->index_ < 0) {
        if (events_map_.find(event->fd_) != events_map_.end()) {
            return;
        }
        event->index_ = 1;
        events_map_[event->Fd()] = event;
        update(EPOLL_CTL_ADD, event);
    }else {
        if(events_map_.find(event->fd_) == events_map_.end()) {
            return;
        }
        
        if(event->events_ == 0) {
            update(EPOLL_CTL_DEL, event);
            event->index_ = 2;
        }else {
            update(EPOLL_CTL_MOD, event);
        }
    }
}

void EPollPoller::RemoveEvent(Event* event) {
    events_map_.erase(event->Fd());
    if(event->index_ == 1) {
        update(EPOLL_CTL_DEL, event);
    }
    event->index_ = -1;
}

Time EPollPoller::Poll(int timeout, std::vector<Event*>& activeEvents) {
    int n = ::epoll_wait(epollfd_,
                         &*epoll_events_.begin(),
                         static_cast<int>(epoll_events_.size()),
                         timeout);
    
    Time now = Time::Now();
    
    for(int i = 0; i < n; ++i) {
        Event* event = static_cast<Event*>(epoll_events_[i].data.ptr);
        event->revents_ = epoll_events_[i].events;
        activeEvents.push_back(event);
    }
    
    return now;
}

void EPollPoller::update(int operation, Event *event) {
    struct epoll_event ev;
    memZero(&ev, sizeof ev);
    ev.events = event->events_;
    ev.data.ptr = event;
    int fd = event->fd();
    
    ::epoll_ctl(epollfd_, operation, fd, &ev)
}

}
}
