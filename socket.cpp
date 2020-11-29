/*************************************************************************
	> File Name: socket.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年07月17日 星期五 09时49分44秒
 ************************************************************************/
#include <iostream>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket.h"
using namespace std;

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

void  Socket::accept_()
{
    conn_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_len);
}


