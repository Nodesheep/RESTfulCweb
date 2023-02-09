#ifndef CWEB_UTIL_NONCOPYABLE_H_
#define CWEB_UTIL_NONCOPYABLE_H_

namespace cweb {

namespace util {

class Noncopyable {
public:
    Noncopyable(const Noncopyable&) = delete;
    void operator=(const Noncopyable&) = delete;

protected:
    Noncopyable() = default;
    ~Noncopyable() = default;
};

}

}

#endif