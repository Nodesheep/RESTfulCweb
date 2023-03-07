#ifndef CWEB_COROUTINE_PROCESSER_H_
#define CWEB_COROUTINE_PROCESSER_H_

#include "../tcpserver/eventloop.h"
#include "../util/linked_list.h"

namespace cweb {
namespace tcpserver {

namespace coroutine {

class Coroutine;
class Scheduler;
class Processer : public EventLoop {
private:
    Scheduler* scheduler_;
    Coroutine* running_coroutine_ = nullptr;
    Coroutine* next_coroutine_ = nullptr;
    util::LinkedList<Coroutine> ready_coroutines_;
    util::LinkedList<Coroutine> wait_coroutines_;
    
    void moveCoroutines();

protected:
    virtual void loop() override;
    
public:
    virtual void Run() override;
    virtual void AddTask(Functor cb) override;
    virtual void AddTasks(std::vector<Functor>& cbs) override;
    
    void AddCoroutine(Coroutine* co);
    static Coroutine* GetMainCoroutine();

};

static __thread Processer* TLSProceser;

}
}
}

#endif
