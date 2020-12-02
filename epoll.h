/*************************************************************************
	> File Name: Epoll.h
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年07月17日 星期五 09时46分13秒
 ************************************************************************/

#ifndef _EPOLL_H
#define _EPOLL_H
#include "threadpool.h"
#define EPOLL_MAX 100000
#define LISTENAMX 1000
#include <sys/epoll.h> 
#include <unistd.h>
#include <fcntl.h>
#include "timer.h"
#include "timer_wheel.h"
#include "conn.h"

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

class Epoll {
public:
    Epoll();
    ~Epoll() {  }
    int active_fd();
    void deal();

    void fd_write(int fd);
    void fd_read(int fd);
    void epoll_mod_(int);
    void epoll_del_(int);
    void epoll_add_(int);

    virtual void call_epollerr(int) = 0;
    virtual void call_epollin(int) = 0;
    virtual void call_epollout(int) = 0;

private: 
    int epoll_fd;
    struct epoll_event ev, events[EPOLL_MAX];
    int fd_num; //活跃的文件描述符数量
   
};


Epoll::Epoll()
{
    epoll_fd = epoll_create(EPOLL_MAX);
}

void Epoll::fd_read(int fd)
{
    ev.data.fd = fd;
    ev.events = EPOLLIN;
}

void Epoll::fd_write(int fd)
{
    ev.data.fd = fd;
    ev.events = EPOLLOUT;
}

void Epoll::epoll_add_(int fd)
{
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    setnonblocking(fd);  
}

void Epoll::epoll_del_(int fd)
{
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev);
}

void Epoll::epoll_mod_(int fd)
{
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}

int Epoll::active_fd()
{
    fd_num = epoll_wait(epoll_fd, events, EPOLL_MAX, 1000);
    return fd_num;
}

void Epoll::deal()
{
    for (int i = 0; i < fd_num; i++) {
        if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
            call_epollerr(events[i].data.fd); 
        }
        else if(events[i].events & EPOLLIN) {
            call_epollin(events[i].data.fd);
        }
        else if (events[i].events & EPOLLOUT) {
            call_epollout(events[i].data.fd);
        }
    }
}
#endif
