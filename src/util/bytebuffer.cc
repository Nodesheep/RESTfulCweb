#include "bytebuffer.h"

#include <sys/uio.h>

namespace cweb {
namespace util {


int ByteBuffer::ReadFd(int fd) {
    char extrabuf[65536];
    
    struct iovec vec[2];
    size_t writable = WritableBytes();
    
    vec[0].iov_base = buffer_ + writeindex_;
    vec[0].iov_len = writable;
    
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    
    int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    
    size_t n = readv(fd, vec, iovcnt);
    
    if(n < 0) {
        return -1;
    }else if(n <= writable) {
        writeindex_ += n;
    }else {
        writeindex_ = size_;
        
        Append(extrabuf, n - writable);
    }
    
    return (int)n;
}

void ByteBuffer::Append(const char *data, size_t len) {
    if(WritableBytes() < len) {
        extendSpace(len);
    }
    memcpy(buffer_ + writeindex_, data, len);
    writeindex_ += len;
}

void ByteBuffer::Append(const StringPiece& str) {
    Append(str.Data(), str.Size());
}

void ByteBuffer::ReadUtil(const char *end) {
    size_t len = end - Peek();
    ReadBytes(len);
}

void ByteBuffer::ReadBytes(size_t len) {
    if(len < ReadableBytes()) {
        readindex_ += len;
    }else {
        readindex_ = kCheapPrepend;
        writeindex_ = kCheapPrepend;
    }
}

void ByteBuffer::WriteBytes(size_t len) {
    writeindex_ += len;
}

void ByteBuffer::ReadAll() {
    readindex_ = kCheapPrepend;
    writeindex_ = kCheapPrepend;
}

const char* ByteBuffer::ReadJSON() {
    int n = 0;
    if(*Peek() == '{') {
        n = 1;
        for(char* iter = buffer_ + readindex_ + 1; iter < buffer_ + writeindex_; ++iter) {
            if(*iter == '}') n--;
            if(*iter == '{') n++;
            if(n == 0) {
                return iter;
            }
        }
    }
    return Peek();
}

const char* ByteBuffer::FindCRLF() {
    for(char* iter = buffer_ + readindex_; iter < buffer_ + writeindex_; ++iter) {
        if(*iter == '\r') {
            if(iter + 1 == buffer_ + writeindex_) {
                return NULL;
            }else if(*(iter + 1) == '\n') {
                return iter;
            }
        }
    }
    return NULL;
}


void ByteBuffer::extendSpace(size_t len) {
    if(WritableBytes() + PrependableBytes() < len + kCheapPrepend) {
        
        char* newbuffer = (char*)malloc(size_ + len);
        memset(newbuffer, 0, size_ + len);
        size_ += len;
        size_t readable = ReadableBytes();
        memcpy(newbuffer + kCheapPrepend, Peek(), readable);
        free(buffer_);
        readindex_ = kCheapPrepend;
        writeindex_ = kCheapPrepend + readable;
        buffer_ = newbuffer;
    }else {
        
        size_t readable = ReadableBytes();
        memmove(buffer_ + kCheapPrepend, buffer_ + readindex_, ReadableBytes());
        readindex_ = kCheapPrepend;
        writeindex_ = readindex_ + readable;
    }
}

}
}
