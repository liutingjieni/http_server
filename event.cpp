/*************************************************************************
	> File Name: event.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年10月30日 星期五 17时31分04秒
 ************************************************************************/

#include <iostream>
#include "socket.h"
#include "epoll.h"
using namespace std;

class event {

private:
    Socket socket;
    Epoll epoll;
};
