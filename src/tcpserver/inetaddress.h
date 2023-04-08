#ifndef CWEB_TCP_INETADDRESS_H_
#define CWEB_TCP_INETADDRESS_H_

#include <arpa/inet.h>
#include <string>

namespace cweb {
namespace tcpserver {

class InetAddress {
public:
    friend class Socket;
    InetAddress() {}
    InetAddress(uint16_t port, bool loopbackonly = false, bool ipv6 = false);
    InetAddress(const std::string& ip, uint16_t port, bool ipv6 = false);
    InetAddress(const struct sockaddr_in addr) : addrv4_(addr) {}
    InetAddress(const struct sockaddr_in6 addr) : addrv6_(addr) {}
    
    //验证加不加&的效果
    void SetSockaddr(const struct sockaddr_in addr) {addrv4_ = addr;}
    void SetSockaddr(const struct sockaddr_in6 addr) {addrv6_ = addr;}
    
    bool IsIPv6() const {return ipv6_;}
    struct sockaddr_in* Addrv4() {return &addrv4_;}
    struct sockaddr_in6* Addrv6() {return &addrv6_;}
    
private:
    union {
        struct sockaddr_in addrv4_;
        struct sockaddr_in6 addrv6_;
    };
    bool ipv6_ = false;
};

}
}

#endif
