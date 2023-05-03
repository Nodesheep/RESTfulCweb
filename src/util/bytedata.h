#ifndef CWEB_UTIL_BYTEDATA_H_
#define CWEB_UTIL_BYTEDATA_H_

#include "bytebuffer.h"
#include <vector>
#include <unistd.h>
#include <sys/mman.h>

namespace cweb {
namespace util {

class DataPacket {
private:
    union DataUnion {
        ByteBuffer* copy_data_;
        char* zero_copy_data_;
    };
    
    DataUnion data_;
    size_t size_ = 0;
    int fd_ = -1;
    bool copy_ = false;
    
public:
    friend class ByteData;
    ~DataPacket() {
        if(copy_) {
            delete data_.copy_data_;
        }else {
            if(fd_ > 0) {
                munmap(data_.zero_copy_data_, size_);
                close(fd_);
            }
            //外部传入的0拷贝内存数据应当自己管理
        }
    }
    
    const char* Data() const {
        if(!copy_) return data_.zero_copy_data_;
        else return data_.copy_data_->Peek();
    }
    
    bool CopyIfNeed(size_t offset = 0) {
        if(copy_ || fd_ > 0) return false;
        copy_ = true;
        size_ -= offset;
        data_.copy_data_ = new ByteBuffer(size_);
        data_.copy_data_->Append(data_.zero_copy_data_ + offset, size_);
        return true;
    }
};

class ByteData {
private:
    std::vector<DataPacket*> datas_;
    size_t current_index_ = 0;
    size_t offset_ = 0;
    
    void modifyIndexAndOffset();
    
public:
    ~ByteData() {
        for(DataPacket* data : datas_) delete data;
    }
    
    //内部不拷贝，注意不要提前释放数据，如果一次性没有发完的数据需要手动调用CopyDataIfNeed方法对数据进行缓存
    void AddDataZeroCopy(const StringPiece& data);
    void AddDataZeroCopy(const void* data, size_t size);
    
    //内部会存在一次拷贝
    void AddDataCopy(const StringPiece& data);
    void AddDataCopy(const void* data, size_t size);
    void AppendData(const void* data, size_t size);
    void AddFile(const std::string& filepath);
    void AddFile(int fd, size_t size);
 
    size_t Writev(int fd);
    bool Remain();
    void CopyDataIfNeed();
};

}
}

#endif 
