#include "websocket.h"
#include "sha1.h"
#include "base64.h"

namespace cweb {
namespace httpserver {

WebSocket::WebSocket(std::shared_ptr<TcpConnection> conn, RequestCallback cb) : HttpSession(conn, cb), data_buffer_(new ByteBuffer){
#ifdef COROUTINE
    buffer_.reset(new ByteBuffer);
#endif
}

WebSocket::~WebSocket() {}

void WebSocket::HandleClose() {
    if(close_callback_) {
        close_callback_(std::dynamic_pointer_cast<WebSocket>(shared_from_this()));
    }
}

void WebSocket::Start(std::unique_ptr<HttpRequest> req) {
    std::string sec_key = req->headers_["Sec-WebSocket-Key"] + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    util::encrypt::SHA1 s;
    s.processBytes(sec_key.data(), sec_key.size());
    uint8_t digest[20];
    s.getDigestBytes(digest);
    
    static std::string header = "HTTP/1.1 101 Switching Protocols\r\n"
                                "Upgrade: websocket\r\n"
                                "Connection: Upgrade\r\n"
                                "Sec-WebSocket-Accept: ";
    std::string response;
    response += header + util::encode::base64encode((char*)digest, 20) + "\r\n\r\n";
    ByteData* bdata = new ByteData();
    bdata->AddDataZeroCopy(response);
    connection_->Send(bdata);
    request_callback_(shared_from_this(), std::move(req));
}

/*
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if Payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - - +-------------------------------+
|                               |Masking-key, if MASK set to 1  |
+-------------------------------+-------------------------------+
| Masking-key (continued)       |          Payload Data         |
+-------------------------------- - - - - - - - - - - - - - - - +
:                     Payload Data continued ...                :
+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
|                     Payload Data continued ...                |
+---------------------------------------------------------------+
 */

TcpConnection::MessageState WebSocket::handleMessage(std::shared_ptr<TcpConnection>, ByteBuffer *buf, Time time) {
    while(true) {
        switch (parse_state_) {
            case MiniHeader: {
                //16位
                if(buf->ReadSome(&mini_header_, 2) > 0) {
                    mini_header_ = ntohs(mini_header_);
                    //第8位 mask
                    if((mini_header_ & 0x80) == 0x80) {
                        if ((mini_header_ & 0x7f) == 127) {
                            parse_state_ = Len64;
                        }else if((mini_header_ & 0x7f) == 126) {
                            parse_state_ = Len16;
                        }else {
                            parse_state_ = Mask;
                            length_needrecv_ = mini_header_ & 0x7f;
                        }
                                                            
                    }else {
                        connection_->ForceClose();
                        return tcpserver::TcpConnection::BAD;
                    }
                }else {
                    return tcpserver::TcpConnection::PROCESS;
                }
            }
                break;
            case Len16: {
                if(buf->ReadSome(&length16_needrecv_, 2) > 0) {
                    length16_needrecv_ = ntohs(length16_needrecv_);
                    length_needrecv_ = length16_needrecv_;
                    parse_state_ = Mask;
                }else {
                    return tcpserver::TcpConnection::PROCESS;
                }
            }
                break;
            case Len64: {
                if(buf->ReadSome(&length_needrecv_, 8) > 0) {
                    length_needrecv_ = ntohll(length_needrecv_);
                    parse_state_ = Mask;
                }else {
                    return tcpserver::TcpConnection::PROCESS;
                }
            }
                break;
            case Mask: {
                if(buf->ReadSome((char*)&mask_, 4)) {
                    parse_state_ = Payload;
                }else {
                    return tcpserver::TcpConnection::PROCESS;
                }
            }
                break;
            case Payload: {
                size_t recvbytes = buf->ReadableBytes();
                if(recvbytes > length_needrecv_) recvbytes = length_needrecv_;
                length_needrecv_ -= buf->ReadToBuffer(data_buffer_.get(), recvbytes);
                if(length_needrecv_ == 0) {
                    recv_end_index = (int)data_buffer_->ReadableBytes();
                    parse_state_ = MiniHeader;
                    handleFragment();
                    if(isFIN()) return tcpserver::TcpConnection::FINISH;
                    else return tcpserver::TcpConnection::PROCESS;
                }else {
                    return tcpserver::TcpConnection::PROCESS;
                }
            }
                break;
            default:
                break;
        }
    }
}

bool WebSocket::isFIN() {
    return mini_header_ & 0x8000;
}

int WebSocket::opcode() {
    return (mini_header_ & 0x0f00) >> 8;
}

//底层关时怎么通知websocket
void WebSocket::handleFragment() {
    for(int i = recv_begin_index_; i < recv_end_index; ++i) {
        (*data_buffer_.get())[i] ^= ((char*)&mask_)[(i - recv_begin_index_) % 4];
    }
    
    recv_begin_index_ = recv_end_index;
    
    switch (opcode()) {
        case 0: {
            if(isFIN()) {
                if(message_callback_) {
                    message_callback_(std::dynamic_pointer_cast<WebSocket>(shared_from_this()), data_buffer_->Peek(), data_buffer_->ReadableBytes(), type_);
                    fragmentFIN();
                }
            }
        }
            break;
            
        case 1: {
            type_ = Text;
            if(isFIN()) {
                if(message_callback_) {
                    message_callback_(std::dynamic_pointer_cast<WebSocket>(shared_from_this()), data_buffer_->Peek(), data_buffer_->ReadableBytes(), type_);
                    fragmentFIN();
                }
            }
        }
            break;
        case 2: {
            type_ = Binary;
            if(isFIN()) {
                if(message_callback_) {
                    message_callback_(std::dynamic_pointer_cast<WebSocket>(shared_from_this()), data_buffer_->Peek(), data_buffer_->ReadableBytes(), type_);
                    fragmentFIN();
                }
            }
        }
            break;
        //close
        case 0x8: {
            recv_close_ = true;
            if(!sent_close) {
                Close();
            }else {
                if(close_callback_) {
                    close_callback_(std::dynamic_pointer_cast<WebSocket>(shared_from_this()));
                    connection_->ForceClose();
                }
            }
        }
            break;
            //ping
        case 0x9: {
            sendPong();
            fragmentFIN();
        }
            break;
        default:
            connection_->ForceClose();
            break;
    }
}

#ifdef COROUTINE
void WebSocket::Send(const void *data, size_t size) {
    SendBinary((const char*)data, size);
}

ssize_t WebSocket::Recv(ByteBuffer* buf) {
    while(true) {
        ssize_t n = connection_->Recv(buffer_.get());
        if(n <= 0 ) return n;
        handleMessage(connection_, buffer_.get(), Time::Now());
        if(isFIN()) {
            data_buffer_->ReadToBuffer(buf, data_buffer_->ReadableBytes());
            recv_begin_index_ = 0;
            recv_end_index = 0;
            return buf->ReadableBytes();
        }
    }
    
}
#endif

void WebSocket::SendText(const StringPiece& str) {
    ByteData* bdata = new ByteData();
    std::string header = buildHeader(0x1, str.Size());
    bdata->AddDataZeroCopy(header);
    bdata->AddDataZeroCopy(str);
    connection_->Send(bdata);
}

void WebSocket::SendBinary(const char *data, size_t size) {
    ByteData* bdata = new ByteData();
    std::string header = buildHeader(0x2, size);
    bdata->AddDataZeroCopy(header);
    bdata->AddDataZeroCopy(data, size);
    connection_->Send(bdata);
}

void WebSocket::Close(const char* data, size_t size) {
    sent_close = true;
    //被动close
    std::string header = buildHeader(0x8, size);
    if(size == 0) {
        connection_->Send(header.data(), header.size());
    }else {
        ByteData* bdata = new ByteData();
        bdata->AddDataZeroCopy(header);
        if(size > 0) {
            bdata->AddDataZeroCopy(data, size);
        }
        connection_->Send(bdata);
    }
    
    if(recv_close_) {
        connection_->ForceClose();
    }
}

void WebSocket::sendPong() {
    char buf[3] = "\x8A\x00"; //0x00
    ByteData* bdata = new ByteData();
    bdata->AddDataZeroCopy(buf, 2);
    connection_->Send(bdata);
}

std::string WebSocket::buildHeader(int opcode, size_t size) {
    char buf[2+8] = "\x80\x00"; //1000000000000000 两字节
    buf[0] += opcode;
    if(size < 126) {
        buf[1] += size; //字节序针对多字节
        return std::string(buf, 2);
    }else if(size < 0x10000) {
        buf[1] += 126;
        *(uint16_t*)(buf+2) = htons((uint16_t)size);
        return std::string(buf, 4);
    }else {
        buf[1] += 127;
        *reinterpret_cast<uint64_t*>(buf+2) = htonll(static_cast<uint64_t>(size));
        return std::string(buf, 10);
    }
}

void WebSocket::fragmentFIN() {
    data_buffer_->ReadAll();
    recv_begin_index_ = 0;
    recv_end_index = 0;
}


}
}
