#include "bytedata.h"
#include "hooks.h"
//#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace cweb {
namespace tcpserver {

void ByteData::AddDataZeroCopy(const StringPiece& data) {
    AddDataZeroCopy(data.Data(), data.Size());
}

void ByteData::AddDataZeroCopy(const void *data, size_t size) {
    DataPacket* dp = new DataPacket();
    dp->copy_ = false;
    dp->zero_copy_data_ = (char*)data;
    dp->size_ += size;
    datas_.push_back(dp);
}

void ByteData::AddDataCopy(const StringPiece &data) {
    AddDataCopy(data.Data(), data.Size());
}

void ByteData::AddDataCopy(const void *data, size_t size) {
    DataPacket* dp = new DataPacket();
    dp->copy_ = true;
    dp->copy_data_ = new ByteBuffer(size * 2);
    dp->copy_data_->Append((const char*)data, size);
    dp->size_ += size;
    datas_.push_back(dp);
}

void ByteData::AppendData(const void *data, size_t size) {
    int len = (int)datas_.size();
    assert(len != 0 && !datas_[len-1]->copy_);
    datas_[len-1]->copy_data_->Append((const char*)data, size);
    datas_[len-1]->size_ += size;
}

void ByteData::AddFile(const std::string &filepath) {
    int fd = open(filepath.c_str(), O_RDONLY);
    assert(fd > 0);
    struct stat st;
    fstat(fd, &st);
    DataPacket* dp = new DataPacket();
    dp->fd_ = fd;
    dp->copy_ = false;
    dp->size_ += st.st_size;
    dp->zero_copy_data_ = (char*)mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(dp->zero_copy_data_ != MAP_FAILED);
    datas_.push_back(dp);
}

void ByteData::AddFile(int fd, size_t size) {
    assert(fd > 0);
    DataPacket* dp = new DataPacket();
    dp->fd_ = fd;
    dp->copy_ = false;
    dp->size_ += size;
    dp->zero_copy_data_ = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(dp->zero_copy_data_ != MAP_FAILED);
    datas_.push_back(dp);
}

ssize_t ByteData::Writev(int fd) {
    if(!Remain()) return 0;
    std::vector<struct iovec> iovs;
    for(int i = (int)current_index_; i < datas_.size(); ++i) {
        struct iovec iov;
        DataPacket* data = datas_[i];
        if(i == (int)current_index_) {
            iov.iov_base = (void*)(data->Data() + offset_);
            iov.iov_len = data->size_ - offset_;
        }else {
            iov.iov_base = (void*)data->Data();
            iov.iov_len = data->size_;
        }
        iovs.push_back(iov);
    }

    ssize_t n = writev(fd, (struct iovec*)(&*iovs.begin()), (int)iovs.size());
    if(n > 0) {
        offset_ += n;
        modifyIndexAndOffset();
    }
    return n;
}

bool ByteData::Remain() {
    return !(current_index_ == datas_.size() - 1 && offset_ == datas_[datas_.size() - 1]->size_);
}

void ByteData::CopyDataIfNeed() {
    if(Remain()) {
        for(int i = (int)current_index_; i < datas_.size(); ++i) {
            if(i == (int)current_index_) {
                if(datas_[i]->CopyIfNeed(offset_)) offset_ = 0;
            }else {
                datas_[i]->CopyIfNeed();
            }
        }
    }
}

void ByteData::modifyIndexAndOffset() {
    int size = (int)datas_.size();
    for(int i = (int)current_index_; i < size; ++i) {
        offset_ -= datas_[i]->size_;
        if(offset_ < 0) {
            offset_ += datas_[i]->size_;
            current_index_ = i;
            return;
        }
    }
    current_index_ = size - 1;
    offset_ = datas_[current_index_]->size_;
    return;
}


}
}
