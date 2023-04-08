#include "co_socket.h"
#include "hooks.h"
#include "inetaddress.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

CoSocket* CoSocket::CreateNonblockFdAndBind(InetAddress* addr) {
    int fd = -1;
    if(addr->IsIPv6()) {
        fd = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    }else {
        fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    
    int ret = 0;
    
    if(addr->IsIPv6()) {
        ret = ::bind(fd, (struct sockaddr*)addr->Addrv6(), static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
    }else {
        ret = ::bind(fd, (struct sockaddr*)addr->Addrv4(), static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    }
    
    CoSocket* socket = new CoSocket(fd);
    //socket->SetNonBlock();
    
    return socket;
}

int CoSocket::Accept(InetAddress *peeraddr) {
    sockaddr addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = static_cast<socklen_t>(sizeof(addr));
    int connfd = hook_accept(fd_, &addr, &len);
    //::accept(<#int#>, <#struct sockaddr *#>, <#socklen_t *#>)
    
    if(connfd  < 0) return -1;
    
    if(addr.sa_family == AF_INET) {
        peeraddr->SetSockaddr(*(sockaddr_in*)&addr);
    }else {
        peeraddr->SetSockaddr(*(sockaddr_in6*)&addr);
    }
    
    return connfd;
}

ssize_t CoSocket::Read(void *buffer, size_t len) {
    return hook_read(fd_, buffer, len);
}

ssize_t CoSocket::Write(const void* buffer, size_t len) {
    return hook_write(fd_, buffer, len);
}

}
}
}
