#ifndef CWEB_TCP_SOCKET_H_
#define CWEB_TCP_SOCKET_H_

#include "../util/noncopyable.h"
#include <sys/types.h>
#include <sys/uio.h>

namespace cweb {
namespace tcpserver {

class InetAddress;
class Socket : public util::Noncopyable {
public:
    Socket() {}
    Socket(int fd) : fd_(fd) {}
    
    int Fd() const {return fd_;}
    
    int Listen();
    int Accept(InetAddress* peeraddr);
    void Close();
    int SetNonBlock();
    int Bind(InetAddress* addr);
    
    ssize_t Read(void* buffer, size_t len);
    ssize_t Write(const void* buffer, size_t len);
    ssize_t Readv(const struct iovec* iov, int iovcnt);
    ssize_t Writev(const struct iovec* iov, int iovcnt);
    ssize_t Recv(void* buffer, size_t len, int flags);
    ssize_t Send(const void* buffer, size_t len, int flags);
    
    static Socket* CreateNonblockFdAndBind(InetAddress* addr);
private:
    int fd_ = -1;
    bool connected_ = false;
    bool nonblock_ = false;
};

}
}

#endif
