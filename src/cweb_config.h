#ifndef CWEB_CWEBCONFIG_H_
#define CWEB_CWEBCONFIG_H_

#include <signal.h>

namespace cweb {

class EnvConfig {
public:
    static void Init() {
        signal(SIGPIPE, SIG_IGN);
    }
};

class LogConfig {
//TODO 配置文件
};

}


#endif
