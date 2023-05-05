#ifndef CWEB_UTIL_PTHREADKEYS_H_
#define CWEB_UTIL_PTHREADKEYS_H_

#include <pthread.h>
#include "singleton.h"

namespace cweb {
namespace util {

class PthreadKeys {
public:
    pthread_key_t TLSEventLoop;
    pthread_key_t TLSMainCoroutine;
    pthread_key_t TLSMemoryPool;
    PthreadKeys() {
        pthread_key_create(&TLSEventLoop, NULL);
        pthread_key_create(&TLSMainCoroutine, NULL);
        pthread_key_create(&TLSMemoryPool, NULL);
    }
};

typedef cweb::util::Singleton<PthreadKeys> PthreadKeysSingleton;

}
}

#endif
