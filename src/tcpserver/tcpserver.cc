#include "tcpserver.h"
#include "inetaddress.h"
#include "socket.h"
#include "event.h"
#include "timer.h"
#include "tcpconnection.h"
#include "eventloop.h"
#include "scheduler.h"
#include "logger.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace cweb::log;
namespace cweb {
namespace tcpserver {

TcpServer::TcpServer(EventLoop* loop, uint16_t port, bool loopbackonly, bool ipv6)
: accept_loop_(loop),
  Server(new InetAddress(port, loopbackonly, ipv6)) {
}

TcpServer::TcpServer(EventLoop* loop, const std::string& ip, uint16_t port, bool ipv6)
: accept_loop_(loop),
  Server(new InetAddress(ip, port, ipv6)) {
}

TcpServer::~TcpServer() {
    if(running_) {
        Quit();
    }
    delete accept_socket_;
    delete accept_event_;
    delete scheduler_;
}

void TcpServer::init() {
    //可以用指针
    accept_socket_ = Socket::CreateNonblockFdAndBind(addr_);
    if(!accept_socket_) {}
    accept_event_ =  new Event(accept_loop_, accept_socket_->Fd());
    accept_event_->SetReadCallback(std::bind(&TcpServer::handleAccept, this));
}

void TcpServer::Start(int threadcnt) {
    init();
    //创建线程池
    running_ = true;
    scheduler_ = new Scheduler(accept_loop_, threadcnt);
    scheduler_->Start();
    if(accept_socket_->Listen() < 0) {}
    accept_event_->EnableReading();
    accept_loop_->Run();
}

void TcpServer::Quit() {
    running_ = false;
    accept_event_->DisableAll();
    accept_event_->Remove();
    scheduler_->Stop();
}

void TcpServer::handleAccept() {
    InetAddress* peeraddr = new InetAddress();
    int connfd = accept_socket_->Accept(peeraddr);
    
    Socket* socket = nullptr;
    if(connfd > 0) socket = new Socket(connfd);
    else {
        LOG(LOGLEVEL_WARN, CWEB_MODULE, "tcpserver", "创建连接失败");
        return;
    }
    
    socket->SetNonBlock();
    EventLoop* loop = scheduler_->GetNextLoop();
    std::string id = boost::uuids::to_string(random_generator_());
    LOG(LOGLEVEL_INFO, CWEB_MODULE, "tcpserver", "创建连接，connfd: %d, id: %s", connfd, id.c_str());
    //创建一个新请求
    TcpConnection* conn = new TcpConnection(loop, socket, peeraddr, id);
    conn->close_callback_ = std::bind(&TcpServer::handleConnectionClose, this, std::placeholders::_1);
    conn->message_callback_ = message_callback_;
    living_connections_[id] = conn;
    loop->AddTask(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::handleConnectionClose(const Connection* conn) {
    accept_loop_->AddTask(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const Connection *conn) {
    living_connections_.erase(((TcpConnection*)conn)->id_);
    delete (TcpConnection*)conn;
}



}
}
