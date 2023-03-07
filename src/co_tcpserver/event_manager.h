#ifndef CWEB_COROUTINE_EVENTMANAGER_H_
#define CWEB_COROUTINE_EVENTMANAGER_H_

#include <unordered_map>
#include "../util/singleton.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

class Event;
class EventManager {
public:
    void AddEvent(int fd, Event* event);
    Event* GetEvent(int fd);
    void RemoveEvent(int fd);

private:
    std::unordered_map<int, Event*> events_;
};

typedef util::Singleton<EventManager> EventManagerSingleton;

}
}
}

#endif

