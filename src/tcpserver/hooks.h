#ifndef CWEB_TCP_HOOKS_H_
#define CWEB_TCP_HOOKS_H_

#include <sys/socket.h>

namespace cweb {
namespace tcpserver {

//去除name mangling
extern "C" {

ssize_t read(int fd, void *buf, size_t nbyte);
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t write(int fd, const void *buf, size_t nbyte);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

int accept(int fd, struct sockaddr *addr, socklen_t *len);
unsigned int sleep(unsigned int seconds);

}

}
}
#endif
