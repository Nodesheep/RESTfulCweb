#ifndef CWEB_DATABASE_CONNECTIONPOOL_H_
#define CWEB_DATABASE_CONNECTIONPOOL_H_

#include "lockfree_queue.h"
#include <memory>
#include <mutex>

using namespace cweb::util;

template <typename T>
class DBConnectionPool {
protected:
    std::shared_ptr<LockfreeQueue<std::shared_ptr<T>>> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;

public:
    DBConnectionPool() {}
    DBConnectionPool(int capcity) : connections_(std::make_shared<LockfreeQueue<std::shared_ptr<T>>>(capcity)) {}
    
    virtual ~DBConnectionPool(){}
    
    std::shared_ptr<T> GetConnection() {
        std::shared_ptr<T> conn;
        std::unique_lock<std::mutex> lock(mutex_);
        while(!connections_->MultiplePop(conn)) {
            cond_.wait(lock);
        }
        
        return conn;
    }
    
    bool ReleaseConnection(std::shared_ptr<T> conn) {
        connections_->MultiplePush(conn);
        cond_.notify_all();
        return true;
    }
};


#endif
