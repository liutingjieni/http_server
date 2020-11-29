/*************************************************************************
	> File Name: socket.h
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年07月17日 星期五 09时36分24秒
 ************************************************************************/

#ifndef _SOCKET_H
#define _SOCKET_H

#define IP "0.0.0.0"
#include <netinet/in.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <utility>
#include <memory>
#include "conn.h"
class Socket{
public:
    Socket(int port);
    Socket(const Socket &sockfd);
    ~Socket() {  }
    int accept_();
    void init(int);
    int get_fd() const { return sock_fd; }
private:
    int sock_fd;
};

Socket::Socket(int port)
{
    init(port);
}

Socket::Socket(const Socket& sock)
{
    sock_fd = sock.sock_fd;
}

void Socket::init(int port)
{
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
    }

    struct sockaddr_in serv_addr;
    int optval = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(int))<0)
    {
        perror("套接字重新绑定失败");

    }

    memset(&serv_addr, 0,sizeof(struct sockaddr_in));
    serv_addr.sin_family =  AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(IP);
    
    if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in))<0)
    {
        perror("绑定失败");
        exit(-1);
    }
    if (listen(sock_fd, 10) < 0)
    {
        perror("设置监听失败");
        exit(-1);
    }
}

int Socket::accept_()
{
    std::shared_ptr<conn> conn_t(new conn);
    conn_t->fd = accept(sock_fd, (struct sockaddr *)&(conn_t->addr), &(conn_t->len));
    conn_list.insert(std::make_pair(conn_t->fd, conn_t));
    return conn_t->fd;
}


#endif
