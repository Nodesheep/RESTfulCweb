#ifndef CWEB_UTIL_ENCODE_H_
#define CWEB_UTIL_ENCODE_H_

#include <string>

namespace cweb {
namespace util {
namespace encode {

inline static std::string base64encode(const char* data, size_t size, const char* key = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/") {
    std::string ret;
    ret.resize((size+2) / 3 * 4);
    auto it = ret.begin();
    while(size >= 3)
    {
        *it++ = key[(((unsigned char)*data)&0xFC)>>2];
        unsigned char h = (((unsigned char)*data++) & 0x03) << 4;
        *it++ = key[h|((((unsigned char)*data)&0xF0)>>4)];
        h = (((unsigned char)*data++) & 0x0F) << 2;
        *it++ = key[h|((((unsigned char)*data)&0xC0)>>6)];
        *it++ = key[((unsigned char)*data++)&0x3F];
        
        size -= 3;
    }
    if (size == 1)
    {
        *it++ = key[(((unsigned char)*data)&0xFC)>>2];
        unsigned char h = (((unsigned char)*data++) & 0x03) << 4;
        *it++ = key[h];
        *it++ = '=';
        *it++ = '=';
    }
    else if (size == 2)
    {
        *it++ = key[(((unsigned char)*data)&0xFC)>>2];
        unsigned char h = (((unsigned char)*data++) & 0x03) << 4;
        *it++ = key[h|((((unsigned char)*data)&0xF0)>>4)];
        h = (((unsigned char)*data++) & 0x0F) << 2;
        *it++ = key[h];
        *it++ = '=';
    }
    return ret;
}

}
}
}

#endif
