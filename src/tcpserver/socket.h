#ifndef CWEB_TCP_SOCKET_H_
#define CWEB_TCP_SOCKET_H_

#include "noncopyable.h"
#include "hooks.h"

namespace cweb {
namespace tcpserver {

class InetAddress;
class Socket : public util::Noncopyable {
public:
    Socket(int fd) : fd_(fd) {connected_ = true;}
    virtual ~Socket();
    
    int Fd() const {return fd_;}
    
    int Listen();
    virtual int Accept(InetAddress* peeraddr);
    void Close();
    int SetNonBlock();
    int Bind(InetAddress* addr);
    
    virtual ssize_t Read(void* buffer, size_t len);
    virtual ssize_t Write(const void* buffer, size_t len);
    virtual ssize_t Readv(const struct iovec* iov, int iovcnt);
    virtual ssize_t Writev(const struct iovec* iov, int iovcnt);
    //virtual ssize_t Recv(void* buffer, size_t len, int flags);
    //virtual ssize_t Send(const void* buffer, size_t len, int flags);
    
    static Socket* CreateFdAndBind(InetAddress* addr, bool nonblock = true);
    
protected:
    int fd_ = -1;
    bool connected_ = false;
    bool nonblock_ = false;
};

}
}

#endif
