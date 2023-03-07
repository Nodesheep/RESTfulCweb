#ifndef CWEB_TCP_POLLER_H_
#define CWEB_TCP_POLLER_H_

#include <vector>
#include <unordered_map>

namespace cweb {
namespace tcpserver {


const int kMaxEventsSize = 1024;

class EventLoop;
class Event;
class Time;
class Poller {
    
public:
    Poller(EventLoop* loop) : ownerloop_(loop) {}
    virtual ~Poller() = default;
    
    virtual void UpdateEvent(Event* event) = 0;
    virtual void RemoveEvent(Event* event) = 0;
    
    virtual Time Poll(uint64_t timeout, std::vector<Event*>& activeEvents) = 0;
    
protected:
    std::unordered_map<int, Event*> events_map_;
    
private:
    EventLoop* ownerloop_;
    
};

}
}

#endif
