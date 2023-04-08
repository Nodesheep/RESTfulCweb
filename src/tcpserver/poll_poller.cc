#include "poll_poller.h"
#include "timer.h"
#include "event.h"

namespace cweb {
namespace tcpserver {

void PollPoller::UpdateEvent(Event* event) {
    if(event->index_ < 0) {
        if (events_map_.find(event->fd_) != events_map_.end()) {
            return;
        }
        
        struct pollfd pfd;
        pfd.fd = event->fd_;
        pfd.events = static_cast<short>(event->events_);
        pfd.revents = 0;
        
        int idx = static_cast<int>(pollfds_.size());
        event->index_ = idx;
        
        pollfds_.push_back(pfd);
        events_map_[pfd.fd] = event;
    }else{
        if(events_map_.find(event->fd_) == events_map_.end()) {
            return;
        }
        
        int idx = event->index_;
        
        struct pollfd& pfd = pollfds_[idx];
        pfd.events = static_cast<short>(event->events_);
        pfd.revents = 0;
    }
}

void PollPoller::RemoveEvent(Event* event) {
    int idx = event->index_;
    //std::cout << "removeEvent" << std::endl;
    if(idx >= 0 && idx < pollfds_.size()) {
        events_map_.erase(event->fd_);
        if((size_t)idx == pollfds_.size()-1) {
            pollfds_.pop_back();
        }else {
            int lastFd = pollfds_.back().fd;
            std::iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);
            if(lastFd < 0) {lastFd = -lastFd - 1;}
            events_map_[lastFd]->index_  = idx;
            pollfds_.pop_back();
        }
        //::close(event->fd());
        //要不要delete
       // delete event;
    }

}

Time PollPoller::Poll(int timeout, std::vector<Event*>& activeEvents) {
    int n = ::poll(&(*pollfds_.begin()), pollfds_.size(), timeout);
    Time now = Time::Now();
    
    if(n > 0) {
        for (std::vector<struct pollfd>::const_iterator pfd = pollfds_.begin(); pfd != pollfds_.end() && n > 0; ++pfd)
            {
                if (pfd->revents > 0)
                {
                    --n;
                    std::unordered_map<int, Event*>::iterator evptr = events_map_.find(pfd->fd);
                    if(evptr != events_map_.end()) {
                        Event* ev = evptr->second;
                        ev->revents_ = pfd->revents;
                        activeEvents.push_back(ev);
                    }
                }
            }
    }
    return now;
}

}
}
