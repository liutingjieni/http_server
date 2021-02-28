/*************************************************************************
	> File Name: conn.h
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年10月19日 星期一 18时53分41秒
 ************************************************************************/

#ifndef _CONN_H
#define _CONN_H
#include "buffer.h"

//保存客户端

//每个连接对应一个conn
class conn {
public:
    conn() : input_(new buffer), output_(new buffer) {  }
    int read();
    string read_buffer();
    string write_buffer();
    int get_fd()
    {
        return fd;
    }
    bool get_linger()
    {
        return linger;
    }
private:
    int fd;
    bool linger;
    struct sockaddr_in addr;
    static socklen_t len;
    std::shared_ptr<buffer> input_;
    std::shared_ptr<buffer> output_;
    int save_errno;
    friend class Socket;
    friend class http;
};

int conn::read()
{
    return input_->read_fd(fd, &save_errno);
}

string conn::read_buffer()
{
    string s(input_->retrieve_all_as_string());
    //cout << "read******"<< s.data() << endl;
    return s;
}

string conn::write_buffer()
{
    string s(output_->retrieve_all_as_string());
    //cout << "write ******"<<s.data() << endl;
    return s;
}

socklen_t conn::len = sizeof(struct sockaddr_in);

//conn_list对应所有连接的信息
std::map<int, std::shared_ptr<conn>> conn_list;  //保存所有的连接信息

#endif
