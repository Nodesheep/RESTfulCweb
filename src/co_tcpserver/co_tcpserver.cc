#include "co_tcpserver.h"
#include "co_scheduler.h"
#include "co_tcpconnection.h"
#include "co_eventloop.h"
#include "co_event.h"
#include "socket.h"
#include "logger.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace cweb::log;

namespace cweb {
namespace tcpserver {
namespace coroutine {

CoTcpServer::CoTcpServer(std::shared_ptr<EventLoop> loop, uint16_t port, bool loopbackonly, bool ipv6)
: TcpServer(loop, port, loopbackonly, ipv6) {}

CoTcpServer::CoTcpServer(std::shared_ptr<EventLoop> loop, const std::string& ip, uint16_t port, bool ipv6)
: TcpServer(loop, ip, port, ipv6) {}

CoTcpServer::~CoTcpServer() {
    
}

void CoTcpServer::Start(int threadcnt) {
    init();
    running_ = true;
    scheduler_.reset(new CoScheduler(std::dynamic_pointer_cast<CoEventLoop>(accept_loop_), threadcnt));
    scheduler_->Start();
    accept_socket_->Listen();
    accept_event_->EnableReading();
    accept_loop_->Run();
}

void CoTcpServer::init() {
    accept_socket_.reset(Socket::CreateFdAndBind(addr_.get(), true));
    accept_event_.reset(new CoEvent(std::dynamic_pointer_cast<CoEventLoop>(accept_loop_), accept_socket_->Fd()));
    accept_event_->SetReadCallback(std::bind(&CoTcpServer::handleAccept, this));
}

void CoTcpServer::handleAccept() {
    while (running_) {
        InetAddress* peeraddr = new InetAddress();

        int connfd = accept_socket_->Accept(peeraddr);
    
        Socket* socket = nullptr;
        if(connfd > 0) socket = new Socket(connfd);
        else {
            LOG(LOGLEVEL_WARN, CWEB_MODULE, "cotcpserver", "创建连接失败");
            delete peeraddr;
            continue;
        }
        
        socket->SetNonBlock();
        std::shared_ptr<CoEventLoop> loop = std::dynamic_pointer_cast<CoEventLoop>(scheduler_->GetNextLoop());
        //底层会调用read
        std::string id = boost::uuids::to_string(random_generator_());
        std::shared_ptr<CoTcpConnection> conn = std::make_shared<CoTcpConnection>(loop, socket, peeraddr, id);
        LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpserver", "创建连接，connfd: %d, id: %s", connfd, id.c_str());
        conn->SetCloseCallback(std::bind(&CoTcpServer::handleConnectionClose, this, std::placeholders::_1));
        conn->SetConnectedCallback(connected_callback_);
        living_connections_[id] = conn;
        loop->AddTask(std::bind(&CoTcpConnection::handleMessage, conn.get()));
    }
}

void CoTcpServer::handleConnectionClose(std::shared_ptr<TcpConnection> conn) {
    accept_loop_->AddTask(std::bind(&CoTcpServer::removeConnectionInLoop, this, conn));
}

void CoTcpServer::removeConnectionInLoop(std::shared_ptr<TcpConnection> conn) {
    LOG(LOGLEVEL_INFO, CWEB_MODULE, "cotcpserver", "移出连接，id: %s", std::dynamic_pointer_cast<CoTcpConnection>(conn) ->id_.c_str());
    living_connections_.erase(std::dynamic_pointer_cast<CoTcpConnection>(conn) ->id_);
}

}
}
}
