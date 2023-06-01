#include "inetaddress.h"
#include <cstring>

namespace cweb {
namespace tcpserver {

InetAddress::InetAddress(uint16_t port, bool loopbackonly, bool ipv6) {
    ipv6_ = ipv6;
    if(ipv6) {
        memset(&addrv6_, 0, sizeof(addrv6_));
        addrv6_.sin6_family = AF_INET6;
        addrv6_.sin6_addr = loopbackonly ? in6addr_loopback : in6addr_any;
        addrv6_.sin6_port = htons(port);
    }else {
        memset(&addrv4_, 0, sizeof(addrv4_));
        addrv4_.sin_family = AF_INET;
        addrv4_.sin_addr.s_addr = htonl(loopbackonly ? INADDR_LOOPBACK : INADDR_ANY);
        addrv4_.sin_port = htons(port);
    }
}

InetAddress::InetAddress(const std::string& ip, uint16_t port, bool ipv6) {
    ipv6_ = ipv6;
    if(ipv6) {
        memset(&addrv6_, 0, sizeof(addrv6_));
        addrv6_.sin6_family = AF_INET6;
        addrv6_.sin6_port = htons(port);
        ::inet_pton(AF_INET6, ip.c_str(), &addrv6_.sin6_addr);
    }else {
        memset(&addrv4_, 0, sizeof(addrv4_));
        addrv4_.sin_family = AF_INET;
        addrv4_.sin_port = htons(port);
        addrv4_.sin_addr.s_addr = htonl(INADDR_ANY);
        //::inet_pton(AF_INET, ip.c_str(), &addrv4_.sin_addr);
    }
}

}
}
