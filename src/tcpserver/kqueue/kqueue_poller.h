#ifndef CWEB_TCP_KQUEUEPOLLER_H_
#define CWEB_TCP_KQUEUEPOLLER_H_

#include "poller.h"
#include <sys/event.h>

namespace cweb {
namespace tcpserver {

class EventLoop;
class KqueuePoller : public Poller {
    
public:
    KqueuePoller(EventLoop* loop);
    ~KqueuePoller();
    
    virtual void UpdateEvent(Event* event) override;
    virtual void RemoveEvent(Event* event) override;
    
    virtual Time Poll(int timeout, std::vector<Event*>& activeEvents) override;
    
private:
    int kqfd_;
    struct kevent* events_;
};
    
}
}
    
#endif
