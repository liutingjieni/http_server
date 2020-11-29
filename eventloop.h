/*************************************************************************
	> File Name: eventloop.h
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年07月17日 星期五 17时01分54秒
 ************************************************************************/

#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H
#include "epoll.h"
class Eventloop {
public:
    Eventloop(Socket fd);
    ~Eventloop();
    

private:
    Epoll epoll_;
}
#endif
