#ifndef CWEB_TCP_POLLPOLLER_H_
#define CWEB_TCP_POLLPOLLER_H_

#include "poller.h"
#include <poll.h>
#include <vector>

namespace cweb {
namespace tcpserver {

class PollPoller : public Poller {
public:
    PollPoller(EventLoop* loop) : Poller(loop){};
    virtual ~PollPoller() {};
    
    virtual void UpdateEvent(Event* event) override;
    virtual void RemoveEvent(Event* event) override;
    
    virtual Time Poll(int timeout, std::vector<Event*>& activeEvents) override;
    
private:
    std::vector<struct pollfd> pollfds_;
    
};

}
}

#endif
