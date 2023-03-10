#include "tcpconnection.h"
#include "eventloop.h"

namespace cweb {
namespace tcpserver {

void TcpConnection::Send(util::ByteBuffer *buf) {
    if(ownerloop_->isInLoopThread()) {
        sendBufferInLoop(buf);
    }else {
        ownerloop_->AddTask(std::bind(&TcpConnection::sendBufferInLoop, this, buf));
    }
}

void TcpConnection::Send(const util::StringPiece &data) {

}

void TcpConnection::handleRead(Time time) {
    
}

void TcpConnection::handleWrite() {
    
}

void TcpConnection::handleClose() {
    
}

void TcpConnection::sendBufferInLoop(util::ByteBuffer *buffer) {
    
}

void TcpConnection::sendInLoop(const void *data, size_t len) {
    
}
}
}

