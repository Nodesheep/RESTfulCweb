#ifndef CWEB_COROUTINE_COEVENTLOOP_H_
#define CWEB_COROUTINE_COEVENTLOOP_H_

#include "eventloop.h"
#include "linked_list.h"
#include <unordered_map>
#include "coroutine.h"
#include <pthread.h>

namespace cweb {
namespace tcpserver {

namespace coroutine {

class CoEvent;
class Coroutine;
class Scheduler;
class CoEventLoop : public EventLoop {
private:
    Scheduler* scheduler_;
    Coroutine* running_coroutine_ = nullptr;
    Coroutine* next_coroutine_ = nullptr;
    Coroutine* main_coroutine_ = nullptr;

    std::unordered_map<int, CoEvent*> events_;
    util::LinkedList<Coroutine> running_coroutines_;
    util::LinkedList<Coroutine> hold_coroutines_;
    util::LinkedList<Coroutine> stateful_ready_coroutines_;
    util::LinkedList<Coroutine> stateless_ready_coroutines_;
    
    void moveReadyCoroutines();

protected:
    void loop();
    void handleActiveEvents(Time time);
    void handleTasks();
    void handleTimeoutTimers();
    
public:
    CoEvent* GetEvent(int fd);
    virtual void Run() override;
    void AddTaskWithState(Functor cb, bool stateful = true);
    void AddCoroutineWithState(Coroutine* co, bool stateful = true);
    virtual void AddTask(Functor cb) override;
    virtual void AddTasks(std::vector<Functor>& cbs) override;
    virtual void UpdateEvent(Event* event) override;
    virtual void RemoveEvent(Event* event) override;
    
    void NotifyCoroutineReady(Coroutine* co);
    Coroutine* GetCurrentCoroutine();
    Coroutine* GetMainCoroutine();

};

}
}
}

#endif
