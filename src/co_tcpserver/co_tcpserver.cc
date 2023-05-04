#include "co_tcpserver.h"
#include "co_socket.h"
#include "co_scheduler.h"
#include "co_tcpconnection.h"
#include "co_eventloop.h"
#include "co_event.h"
#include "logger.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace cweb::log;

namespace cweb {
namespace tcpserver {
namespace coroutine {

CoTcpServer::CoTcpServer(CoEventLoop* loop, uint16_t port, bool loopbackonly, bool ipv6)
: TcpServer(loop, port, loopbackonly, ipv6) {}

CoTcpServer::CoTcpServer(CoEventLoop* loop, const std::string& ip, uint16_t port, bool ipv6)
: TcpServer(loop, ip, port, ipv6) {}

CoTcpServer::~CoTcpServer() {
    
}

void CoTcpServer::Start(int threadcnt) {
    init();
    running_ = true;
    scheduler_ = new CoScheduler((CoEventLoop*)accept_loop_, threadcnt);
    scheduler_->Start();
    accept_socket_->Listen();
    accept_event_->EnableReading();
    accept_loop_->Run();
}


void CoTcpServer::init() {
    accept_socket_ = CoSocket::CreateNonblockFdAndBind(addr_, false);
    accept_event_ =  new CoEvent((CoEventLoop*)accept_loop_, accept_socket_->Fd());
    accept_event_->SetReadCallback(std::bind(&CoTcpServer::handleAccept, this));
}

void CoTcpServer::handleAccept() {
    while (running_) {
        InetAddress* peeraddr = new InetAddress();

        int connfd = accept_socket_->Accept(peeraddr);
    
        CoSocket* socket = nullptr;
        if(connfd > 0) socket = new CoSocket(connfd);
        else {
            LOG(LOGLEVEL_WARN, CWEB_MODULE, "cotcpserver", "创建连接失败");
            delete peeraddr;
            continue;
        }
        
        CoEventLoop* loop = (CoEventLoop*)scheduler_->GetNextLoop();
        std::string id = boost::uuids::to_string(random_generator_());
        
        CoTcpConnection* conn = new CoTcpConnection(loop, socket, peeraddr, id);
        LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpserver", "创建连接，connfd: %d, id: %s", connfd, id.c_str());
        conn->close_callback_ = std::bind(&CoTcpServer::handleConnectionClose, this, std::placeholders::_1);
        conn->message_callback_ = message_callback_;
        living_connections_[id] = conn;
        loop->AddTask(std::bind(&CoTcpConnection::handleMessage, conn));
    }
}

void CoTcpServer::handleConnectionClose(const Connection *conn) {
    accept_loop_->AddTask(std::bind(&CoTcpServer::removeConnectionInLoop, this, conn));
}

void CoTcpServer::removeConnectionInLoop(const Connection *conn) {
    LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpserver", "移出连接，id: %s", ((CoTcpConnection*)conn)->id_.c_str());
    living_connections_.erase(((CoTcpConnection*)conn)->id_);
    delete (CoTcpConnection*)conn;
}

}
}
}
