#ifndef CWEB_TCP_EPOLLPOLLER_H_
#define CWEB_TCP_EPOLLPOLLER_H_

#include "poller.h"
#include <poll.h>
#include <vector>
#include <sys/epoll.h>

namespace cweb {
namespace tcpserver {

class EPollPoller : public Poller {
public:
    EPollPoller(EventLoop* loop);
    virtual ~EPollPoller();
    
    virtual void UpdateEvent(Event* event) override;
    virtual void RemoveEvent(Event* event) override;
    
    virtual Time Poll(int timeout, std::vector<Event*>& activeEvents) override;
    
private:
    int epollfd_;
    std::vector<struct epoll_event> epoll_events_;
    void update(int operation, Event* event);
    
};

}
}

#endif

