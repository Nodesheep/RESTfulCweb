#ifndef CWEB_TCP_INETADDRESS_H_
#define CWEB_TCP_INETADDRESS_H_

#include <arpa/inet.h>
#include <string>

namespace cweb {
namespace tcpserver {

class InetAddress {
public:
    InetAddress(uint16_t port = 0, bool loopbackonly = false, bool ipv6 = false);
    InetAddress(const std::string& ip, uint16_t port, bool ipv6 = false);
    
private:
    union {
        struct sockaddr_in addrv4_;
        struct sockaddr_in6 addrv6_;
    };
};

}
}

#endif
