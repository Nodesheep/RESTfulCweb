#ifndef CWEB_UTIL_BYTEBUFFER_H_
#define CWEB_UTIL_BYTEBUFFER_H_

#include <string>

namespace cweb {
namespace util {

static const size_t kCheapPrepend = 8;
static const size_t kInitialSize = 1024;

class StringPiece {
private:
    const char* ptr_;
    size_t length_;

public:
    StringPiece(const char* str)
        : ptr_(str), length_(static_cast<size_t>(strlen(ptr_))) {}
    
    StringPiece(const std::string& str)
        : ptr_(str.data()), length_(static_cast<size_t>(str.size())) {}
    
    const char* Data() const {return ptr_;}
    
    size_t Size() const {return length_;}
    
};

class ByteBuffer {
private:
    size_t readindex_;
    size_t writeindex_;
    char* buffer_;
    size_t size_;
    
    void extendSpace(size_t len);
    
public:
    explicit ByteBuffer(size_t initialSize = kInitialSize)
    :readindex_(kCheapPrepend),
     writeindex_(kCheapPrepend),
     size_(kCheapPrepend + kInitialSize){
         buffer_ = (char *)malloc((kCheapPrepend + kInitialSize) * sizeof(char));
         memset(buffer_, 0, kCheapPrepend + kInitialSize);
     }
    
    ~ByteBuffer() {
        free(buffer_);
    }
    
    size_t WritableBytes() const {return size_ - writeindex_;}
    size_t ReadableBytes() const {return writeindex_ - readindex_;}
    size_t PrependableBytes() const {return readindex_;}
    
    const char* Peek() const {return buffer_ + readindex_;}
    const char* Back() const {return buffer_ + writeindex_;}
    
    int ReadFd(int fd);
    void ReadUtil(const char* end);
    void ReadBytes(size_t len);
    void WriteBytes(size_t len);
    void ReadAll();
    
    const char* ReadJSON();
    const char* FindCRLF();
    
    void Append(const char* data, size_t len);
    void Append(const StringPiece& str);
};

}
}

#endif
