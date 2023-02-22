#ifndef CWEB_COROUTINE_PROCESSER_H_
#define CWEB_COROUTINE_PROCESSER_H_

#include "../util/runloop.h"
#include "../util/linked_list.h"

namespace cweb {

namespace coroutine {

class Coroutine;
class Scheduler;
class Processer : public util::RunLoop {
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
    void AddCoroutine(Coroutine* co);
    static Coroutine* GetMainCoroutine();

};

}
}

#endif
