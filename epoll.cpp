/*************************************************************************
	> File Name: epoll.cpp
	> Author:jieni 
	> Mail:mZ
	> Created Time: 2020年07月17日 星期五 11时35分18秒
 ************************************************************************/

#include "epoll.h"

Epoll::Epoll(Socket fd) : sock_fd(fd)
{
    epoll_fd = epoll_create(EPOLL_MAX);
    fd_read(sock_fd.get_fd());
    epoll_add_(sock_fd.get_fd());
}

void Epoll::fd_read(int fd)
{
    ev.data.fd = fd;
    ev.events = EPOLLIN;

}
void Epoll::epoll_add_(int fd)
{
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

void Epoll::active_fd()
{
    fd_num = epoll_wait(epoll_fd, events, EPOLL_MAX, 1000);
}

void Epoll::deal()
{
    for (int i = 0; i < fd_num; i++) {
        if(events[i].data.fd == sock_fd.get_fd()) {
            sock_fd.accept_();
            fd_read(sock_fd.get_clifd());
            epoll_add_(sock_fd.get_clifd());
        }
        else if(events[i].events & EPOLLIN) {
            recv(events[i].data.fd, &pack, sizeof(pack) ,MSG_WAITALL);    
            threadpool.push_back(mess_callback_);
        }
    }
}
