//#ifdef MACOS

#include "kqueue_poller.h"
#include "timer.h"
#include "event.h"
#include <unistd.h>

namespace cweb {
namespace tcpserver {

KqueuePoller::KqueuePoller(EventLoop* loop) : Poller(loop) {
    events_ = (struct kevent*)malloc(sizeof(struct kevent) * kMaxEventsSize);
    kqfd_ = kqueue();
}

KqueuePoller::~KqueuePoller() {
    free(events_);
    ::close(kqfd_);
}

Time KqueuePoller::Poll(int timeout, std::vector<Event *>& activeEvents) {
    
    int ret = kevent(kqfd_, NULL, 0, events_, 1024, nullptr);
    Time now = Time::Now();
    
    if(ret > 0) {
        std::unordered_map<int, Event*> active_events;
        for(int i = 0; i < ret; ++i) {
            struct kevent* ev = events_ + i;
            //kqueue读写事件是分开的，避免重复添加
            std::unordered_map<int, Event*>::iterator evptr = active_events.find((int)(ev->ident));
            
            if(evptr != active_events.end()) {
                Event* event = evptr->second;
                if(ev->filter == EVFILT_READ) event->AddRevents(READ_EVENT);
                else if(ev->filter == EVFILT_WRITE) event->AddRevents(WRITE_EVENT);
                continue;
            }
            
            evptr = events_map_.find((int)(ev->ident));
            
            if(evptr != events_map_.end()) {
                Event* event = evptr->second;
                if(ev->filter == EVFILT_READ) event->SetRevents(READ_EVENT);
                else if(ev->filter == EVFILT_WRITE) event->SetRevents(WRITE_EVENT);
                active_events[event->fd_] = event;
            }
        }
        
        for(std::unordered_map<int, Event*>::iterator ptr = active_events.begin(); ptr != active_events.end(); ++ptr) {
            activeEvents.push_back(ptr->second);
        }
    }
    
    return now;
}

void KqueuePoller::UpdateEvent(Event* event) {
    
    struct kevent ke;
    if(event->events_ & READ_EVENT) {
        EV_SET(&ke, event->fd_, EVFILT_READ, EV_ADD, 0, 0, NULL);
        kevent(kqfd_, &ke, 1, NULL, 0, NULL);
    }
    
    if(event->events_ & WRITE_EVENT) {
        EV_SET(&ke, event->fd_, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        kevent(kqfd_, &ke, 1, NULL, 0, NULL);
    }
    events_map_[event->fd_] = event;
}

void KqueuePoller::RemoveEvent(Event* event) {
    
    struct kevent ke;
    if(event->events_ & READ_EVENT) {
        EV_SET(&ke, event->fd_, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        kevent(kqfd_, &ke, 1, NULL, 0, NULL);
    }
    
    if(event->events_ & WRITE_EVENT) {
        EV_SET(&ke, event->fd_, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        kevent(kqfd_, &ke, 1, NULL, 0, NULL);
    }
    
    if(events_map_.find(event->fd_) != events_map_.end()) {
        events_map_.erase(event->fd_);
    }
}

}
}

//#endif
