#ifndef CWEB_LOG_LOGWRITER_H_
#define CWEB_LOG_LOGWRITER_H_

#include <functional>
#include <mutex>
#include <vector>
#include "threadlocal_memorypool.h"
#include "logfile_pipe.h"

namespace cweb {
namespace log {

class LogWriter {
    
private:
    typedef std::function<void()> Functor;
    bool running_ = false;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::vector<Functor> tasks_;
    util::MemoryPool* memorypool_ = new util::MemoryPool();
    LogfilePipe* logfilepipe_ = nullptr;
    
    void loop();
    
public:
    LogWriter(int capacity = 2000);
    void AddTask(Functor cb);
    void Run();
    void Stop();
    void Wakeup();
    void Sleep();
    LogInfo* AllocLogInfo();
    void DeallocLogInfo(LogInfo* info);
};

}
}

#endif
