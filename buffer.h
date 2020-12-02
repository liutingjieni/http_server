/*************************************************************************
	> File Name: buffer.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年10月09日 星期五 14时21分07秒
 ************************************************************************/

#include <iostream>
#include <vector>
#include <assert.h>
#include <sys/uio.h>
#include <unistd.h>

using namespace std;
class buffer {
public:
    static const size_t k_cheap_prepend = 8;
    static const size_t k_init_size = 1024;

    //初始化buffer, buffer被两个指针划分为三个空间, prepend, readable, writable
    explicit buffer(size_t init_size = k_init_size) 
    : buffer_(k_cheap_prepend + init_size),
      read_index_(k_cheap_prepend),
      write_index_(k_cheap_prepend)
    {
        //assert()
    }
    

    size_t read_fd(int fd, int *saved_errno);
    string retrieve_all_as_string();

protected: 
    size_t readable_bytes() const; 
    size_t writable_bytes() const;
    size_t prependable_bytes() const;
    const char*begin() const ;
    char *begin();
    char *begin_write();
    const char *begin_write() const;
    const char *peek() const;
    void make_space(size_t len);
    void ensure_writable_bytes(size_t len);
    void has_written(size_t len);
    void append(const char *data, size_t len);
    void append(const void *data, size_t len);
    void retrieve_all();
    void retrieve(size_t len);
    string retrieve_as_string(size_t len);
private:
    vector<char> buffer_;
    size_t read_index_;
    size_t write_index_;

    friend class http;
};

//通过指针的差值->readable空间的大小
size_t buffer::readable_bytes() const 
{
    return write_index_ - read_index_;
}

//通过指针的差值->writable空间的大小
size_t buffer::writable_bytes() const
{
    return buffer_.size() - write_index_;
}

//通过指针的差值->prependable空间的大小
size_t buffer::prependable_bytes() const
{
    return read_index_;
}

//buffer的开始的迭代器
char * buffer::begin()
{
    return &*buffer_.begin();
}

//const版本
const char* buffer::begin() const 
{
    return &*buffer_.begin();
}

//判断writable写空间的大小与待写入的大小, 如果空间不够make_space, 够了直接追加
void buffer::ensure_writable_bytes(size_t len)
{
    if (writable_bytes() < len) {
        make_space(len);
    }
    assert(writable_bytes() >= len); //确保分配空间或移动空间成功, 能够放入待放的数据
}

//返回writable的首地址
char *buffer::begin_write()
{
    return begin() + write_index_;
}

//const版本
const char *buffer::begin_write() const
{
    return begin() + write_index_;
}

//移动write_index_
void buffer::has_written(size_t len)
{
    assert(len <= writable_bytes());
    write_index_ += len;
}

//收到数据包追加在buffer后面, 判断writeable的空间是否大于要写入的数据空间, 如果是直接写入, 否则移动readable或者重新分配空间
void buffer::append(const char *data, size_t len)
{
    ensure_writable_bytes(len);
    copy(data, data + len, begin_write()); //复制要写入的数据到buffer中
    has_written(len);
}

void buffer::append(const void *data, size_t len)
{
    append(static_cast<const char *>(data), len);
}

// writable不够的时候
void buffer::make_space(size_t len)
{
    //buffer重新分配空间, 是使用之前vector多于的空间还是重新分配都有vector容器操作
    if (writable_bytes() + prependable_bytes() < len + k_cheap_prepend) {
        buffer_.resize(write_index_ + len);
    }
    else { //prepend + writable > 要写入的数据长度, 移动readable 
        assert(k_cheap_prepend < read_index_);
        size_t readable = readable_bytes();
        copy(begin() + read_index_, begin() + write_index_, begin() + k_cheap_prepend);
        read_index_ = k_cheap_prepend;
        write_index_ = read_index_ + readable;
        assert(readable == readable_bytes());
    }
}

// 返回readable的首地址
const char *buffer::peek() const
{
    return begin() + read_index_;
}

//要读取所有的readable的数据
void buffer::retrieve_all()
{
    read_index_ = k_cheap_prepend;
    write_index_ = k_cheap_prepend;
}

// 判断len的大小, 处理read_index_
void buffer::retrieve(size_t len)
{
    assert(len <= readable_bytes());
    if (len < readable_bytes()) {
        read_index_ += len;
    } else {
        retrieve_all();
    }
}

//将要读取的数据放入到result当中去
string buffer::retrieve_as_string(size_t len)
{
    assert(len <= readable_bytes());
    string result(peek(), len);
    retrieve(len);
    return result;
}

string buffer::retrieve_all_as_string()
{
    return retrieve_as_string(readable_bytes());
}

//利用临时栈上空间,避免了每个连接的初始Buffer过大造成的内存浪费, 
//也避免反复调用read()的系统调用(由于缓冲区足够大, 通常一次readv()系统调用就能够读完所有数据)
size_t buffer::read_fd(int fd, int *saved_errno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writable_bytes();
    vec[0].iov_base= begin() + write_index_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const size_t n = readv(fd, vec, iovcnt);
    if (n < 0) {
        *saved_errno = errno;
        cout << "errno" << endl;
    } else if (static_cast<size_t>(n) <= writable) {
        write_index_ += n;
    } else {
        write_index_ = buffer_.size();
        append(extrabuf, n - writable);
    }
    return n;
}

