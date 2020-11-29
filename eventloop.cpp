/*************************************************************************
	> File Name: eventloop.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年07月17日 星期五 19时24分26秒
 ************************************************************************/
#include "eventloop.h"

Eventloop::Eventloop(Socket fd) : epoll_(fd)
{
    while (1) {
        epoll_.active_fd();
        epoll_.deal();
    } 
}
