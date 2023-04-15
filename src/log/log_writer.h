#ifndef CWEB_LOG_LOGWRITER_H_
#define CWEB_LOG_LOGWRITER_H_

#include <functional>
#include <mutex>
#include <vector>

namespace cweb {
namespace log {

class LogWriter {
    
private:
    typedef std::function<void()> Functor;
    bool running_ = false;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::vector<Functor> tasks_;
    
    void loop();
    
public:
    void AddTask(Functor cb);
    void Run();
    void Stop();
    void Wakeup();
    void Sleep();
};

}
}

#endif
