#ifndef CWEB_CWEBCONFIG_H_
#define CWEB_CWEBCONFIG_H_

#include <signal.h>
#include <string>
#include "log_info.h"

namespace cweb {

class EnvConfig {
public:
    static void Init() {
        signal(SIGPIPE, SIG_IGN);
    }
};

class LogConfig {
public:
    std::string log_pattern = "[%d][%l][%T][%m][%t][%c]%n";
    std::string log_filepath = "../logfile";
    cweb::log::LogLevel log_level = cweb::log::LogLevel::LOGLEVEL_DEBUG;
    int writer_capcity = 2000;
    //是否控制台打印日志
    bool need_console = true;
};

class RedisConfig {
public:
    enum Type {
        Standalone,
        Cluster
    };
    
    Type type = Cluster;
    std::string host = "127.0.0.1";
    int port = 8300;
    std::string addrs = "127.0.0.1:8306,127.0.0.1:8307,127.0.0.1:8308";
    std::string password = "12345678";
    int capcity = 5;
    uint64_t timeout_ms = 100;
    
};

class MysqlConfig {
public:
    std::string host = "127.0.0.1";
    int port = 3306;
    std::string user = "root";
    std::string password = "ccw257yej";
    std::string dbname = "test";
    int capcity = 5;
    uint64_t timeout_ms = 100;
};

class ElasticSearchConfig {
    
};

}


#endif
