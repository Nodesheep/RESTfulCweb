#ifndef CWEB_COROUTINE_COSOCKET_H_
#define CWEB_COROUTINE_COSOCKET_H_

#include "../tcpserver/socket.h"

namespace cweb {
namespace tcpserver {
namespace coroutine {

class CoSocket : public Socket {
public:
    CoSocket(int fd) : Socket(fd) {}
    virtual ~CoSocket() {}
    
    virtual int Accept(InetAddress* peeraddr) override;
    virtual ssize_t Read(void* buffer, size_t len) override;
    virtual ssize_t Write(const void* buffer, size_t len) override;
//    ssize_t Readv(const struct iovec* iov, int iovcnt);
//    ssize_t Writev(const struct iovec* iov, int iovcnt);
//    ssize_t Recv(void* buffer, size_t len, int flags);
//    ssize_t Send(const void* buffer, size_t len, int flags);
    
    static CoSocket* CreateNonblockFdAndBind(InetAddress* addr);
};

}
}
}

#endif
