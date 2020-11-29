/*************************************************************************
	> File Name: main.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年07月17日 星期五 15时16分27秒
 ************************************************************************/

#include <iostream>
#include "event.h"
#include <string.h>
#include "http.h"
using namespace std;
using std::placeholders::_1;

event* event_(new event(8888));

void onmessage(shared_ptr<conn> conn_)
{ 
    http *http_(new http(conn_));
    if(http_->process()) {
        event_->fd_write(conn_->get_fd()); 
        event_->epoll_mod_(conn_->get_fd());
    }
    delete http_;
}

//void ontime(shared_ptr<Conn> conn)
//{
//    printf("on time\n");
//}

int main()
{
    event_->set_mess_callback(bind(onmessage, _1));
    event_->loop();
    delete event_;
}
