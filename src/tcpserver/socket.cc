#include "socket.h"
#include <fcntl.h>
#include <sys/socket.h>
#include "inetaddress.h"
#include <unistd.h>

static const int kMaxConnCount = 128;
namespace cweb {
namespace tcpserver {

Socket* Socket::CreateNonblockFdAndBind(InetAddress* addr) {
    int fd = -1;
    if(addr->ipv6_) {
        fd = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    }else {
        fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    
    int ret = 0;
    
    if(addr->ipv6_) {
        ret = ::bind(fd, (struct sockaddr*)&addr->addrv6_, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
    }else {
        ret = ::bind(fd, (struct sockaddr*)&addr->addrv4_, static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    }
    
    Socket* socket = new Socket(fd);
    socket->SetNonBlock();
    
    return socket;
}

int Socket::Listen() {
    int ret = ::listen(fd_, kMaxConnCount);
    return ret;
}

int Socket::Accept(InetAddress* peeraddr) {
    sockaddr addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = static_cast<socklen_t>(sizeof(addr));
    int connfd = ::accept(fd_, &addr, &len);
    
    if(connfd  < 0) return -1;
    
    if(addr.sa_family == AF_INET) {
        peeraddr->SetSockaddr(*(sockaddr_in*)&addr);
    }else {
        peeraddr->SetSockaddr(*(sockaddr_in6*)&addr);
    }
    
    return connfd;
}

void Socket::Close() {
    if(!connected_ || fd_ == -1) return;
    ::close(fd_);
    fd_ = -1;
    connected_ = false;
}

ssize_t Socket::Read(void *buffer, size_t len) {
    return ::read(fd_, buffer, len);
}

ssize_t Socket::Write(const void *buffer, size_t len) {
    return ::write(fd_, buffer, len);
}

ssize_t Socket::Readv(const struct iovec *iov, int iovcnt) {
    return ::readv(fd_, iov, iovcnt);
}

ssize_t Socket::Writev(const struct iovec *iov,int iovcnt) {
    return ::writev(fd_, iov, iovcnt);
}

ssize_t Socket::Recv(void *buffer, size_t len, int flags) {
    return ::recv(fd_, buffer, len, flags);
}

ssize_t Socket::Send(const void *buffer, size_t len, int flags) {
    return ::send(fd_, buffer, len, flags);
}

int Socket::SetNonBlock() {
    if(nonblock_) return 0;
    else {
        int flags = ::fcntl(fd_, F_GETFL, 0);
        flags |= O_NONBLOCK;
        int ret = ::fcntl(fd_, F_SETFL, flags);
        if(ret < 0) return -1;
        else {
            nonblock_ = true;
            return 0;
        }
    }
}

int Socket::Bind(InetAddress *addr) {
    return 0;
}

}
}
