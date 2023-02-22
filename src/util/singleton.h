#ifndef CWEB_UTIL_SINGLETON_H_
#define CWEB_UTIL_SINGLETON_H_

#include "noncopyable.h"

namespace cweb {

namespace util {

template <typename T>
class Singleton : public Noncopyable {

public:

    static T* GetInstance() {
        static T v;
        return &v;
    }

};
}

}

#endif