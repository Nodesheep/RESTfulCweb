#ifndef CWEB_HTTP_WEBSOCKET_H_
#define CWEB_HTTP_WEBSOCKET_H_

#include "httpsession.h"

namespace cweb {
namespace httpserver {

enum MessageType {
    Text,
    Binary
};

class WebSocket : public HttpSession {
    
private:
    typedef std::function<void(std::shared_ptr<WebSocket>, const char*, size_t, MessageType)> MessageCallback;
    typedef std::function<void(std::shared_ptr<WebSocket>)> CloseCallback;
    enum ParseState {
        MiniHeader,
        Len16,
        Len64,
        Mask,
        Payload,
    };
    
    uint32_t mask_;
    uint16_t mini_header_;
    uint16_t length16_needrecv_;
    uint64_t length_needrecv_;
    std::unique_ptr<ByteBuffer> data_buffer_;
    int recv_begin_index_ = 0;
    int recv_end_index = 0;
    ParseState parse_state_ = MiniHeader;
    MessageType type_ = Binary;
    MessageCallback message_callback_;
    CloseCallback close_callback_;
    bool recv_close_ = false;
    bool sent_close = false;
    void sendPong();
    bool isFIN();
    int opcode();
    void handleFragment();
    std::string buildHeader(int opcode, size_t size);
    void fragmentFIN();
public:
    WebSocket(std::shared_ptr<TcpConnection> conn, RequestCallback cb);
    virtual ~WebSocket();
    virtual TcpConnection::MessageState handleMessage(std::shared_ptr<TcpConnection> conn, ByteBuffer* buf, Time time) override;
    void Start(std::unique_ptr<HttpRequest> req);
    void SetMessageCallback(MessageCallback cb) {message_callback_ = std::move(cb);}
    void SetCloseCallback(CloseCallback cb) {close_callback_ = std::move(cb);}
    void SendBinary(const char* data, size_t size);
    void SendText(const StringPiece& str);
    void Close(const char* data = "", size_t size = 0);
    void HandleClose();
    
#ifdef COROUTINE
    void Send(const void* data, size_t size);
    ssize_t Recv(ByteBuffer* buf);
private:
    std::unique_ptr<ByteBuffer> buffer_;
#endif

};

}
}

#endif
