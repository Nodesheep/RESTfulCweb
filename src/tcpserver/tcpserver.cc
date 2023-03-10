#include "tcpserver.h"
#include "inetaddress.h"
#include "socket.h"
#include "event.h"
#include "timer.h"
#include "tcpconnection.h"
#include "eventloop.h"

namespace cweb {
namespace tcpserver {

TcpServer::TcpServer(EventLoop* loop, uint16_t port, bool loopbackonly, bool ipv6)
: accept_loop_(loop),
  addr_(new InetAddress(port, loopbackonly, ipv6)) {
      init();
}

TcpServer::TcpServer(EventLoop* loop, const std::string& ip, uint16_t port, bool ipv6)
: accept_loop_(loop),
  addr_(new InetAddress(ip, port, ipv6)) {
      init();
}

void TcpServer::Start(int threadcnt) {
    //创建线程池
    
    
}

void TcpServer::init() {
    //可以用指针
    accept_socket_ = Socket::CreateNonblockFdAndBind(addr_);
    accept_event_ =  new Event(accept_loop_, accept_socket_->Fd());
    accept_event_->SetReadCallback(std::bind(&TcpServer::handleAccept, this));
}

void TcpServer::handleAccept() {
    InetAddress* peeraddr = new InetAddress();
    int connfd = accept_socket_->Accept(peeraddr);
    
    Socket* socket = nullptr;
    if(connfd > 0) socket = new Socket(connfd);
    else return;
    
    EventLoop* loop = nullptr;
    std::string id = "";
    //创建一个新请求
    TcpConnection* conn = new TcpConnection(loop, socket, peeraddr, id);
    conn->close_callback_ = std::bind(&TcpServer::handleConnectionClose, this, std::placeholders::_1);
    //conn->message_callback_ =
    living_connections_[id] = conn;
}

void TcpServer::handleConnectionClose(const TcpConnection* conn) {
    accept_loop_->AddTask(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnection *conn) {
    living_connections_.erase(conn->id_);
    delete conn;
}



}
}
