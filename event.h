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
using std::placeholders::_1;


void ontime(std::shared_ptr<conn> conn)
{
    printf("on time\n");
    close(conn->get_fd());
    conn_list.erase(conn->get_fd());

}

class event : public Epoll{
public:
    event(int);
    void loop();
    void set_mess_callback(callback  cb) {
        task_.callback_ = cb;
    }
    virtual void call_epollerr(int);
    virtual void call_epollin(int);
    virtual void call_epollout(int);
private:
    Socket sock_fd;
    Task task_;
    Threadpool threadpool;
    Timer timer;
    time_wheel wheel;
};

event::event(int port) : sock_fd(port)
{
    fd_read(sock_fd.get_fd());
    epoll_add_(sock_fd.get_fd());
    fd_read(timer.get_fd());
    epoll_add_(timer.get_fd());
}

void event::call_epollerr(int fd)
{
    cout << "EPOLLERR" << endl;
    epoll_del_(fd);
    close(fd);
    conn_list.erase(fd);

}

void event::call_epollin(int fd)
{
    if(fd == sock_fd.get_fd()) {
        //cout << events[i].data.fd << "accept" << endl;
        int conn_fd = sock_fd.accept_();
        fd_read(conn_fd);
        epoll_add_(conn_fd);

        //在时间轮上为新的连接加tw_timer
        tw_timer *tw = wheel.add_timer(conn_list[conn_fd], 60);
        //为每个到时的连接添加回调函数
        tw->set_time_callback(bind(ontime, _1));
    }
    //定时器事件
    else if (fd == timer.get_fd()) {
        //cout << "epoll_timerfd" << endl;
        char buf[10];
        read(timer.get_fd(), buf, sizeof(buf));
        wheel.tick(); //心跳事件到达
        
        timer.reset(); //定时器重新设置时间
    } 
    //收到包
    else {
        //根据到达连接的fd信息, 找到对应的conn
        shared_ptr<conn> conn = conn_list[fd];
        int ret = conn->read();
        if (ret <= 0) {
            //cout << "ret = 0" << endl;
            epoll_del_(fd);
            wheel.del_timer(conn);
            conn_list.erase(fd);
            close(fd);
        }
        //根据conn的信息在map中找到对应tw_timer, 然后更新他在时间轮上的位置
        wheel.update_timer(conn, 60);

        //在conn_list(所有连接map)找到所对应根据key(fd) 
        task_.conn_ = conn;
        threadpool.push_back(task_);
    }
}

void event::call_epollout(int fd)
{
    shared_ptr<conn> conn = conn_list[fd];
    string s(conn->write_buffer());
    const char *data = s.data();
    cout << "#############"  << " "<< fd << " "<< strlen(data)<< endl;
    int n = write(fd, data, strlen(data));
    if(!conn->get_linger()) {
        cout << "del^^^^^^^^^^^^^^^^^^^^^^" << endl;
        epoll_del_(fd);
        wheel.del_timer(conn);
        conn_list.erase(fd);
        close(fd);
        cout << "del &&&&&&&&&&&&&&&&&&&&" << endl;
    }
    else {
        cout << "mod^^^^^^^^^^^^^^^^^^^^^^" << endl;
        fd_read(fd);
        epoll_mod_(fd);        
    }
}



void event::loop()
{
    while(1) {
        active_fd();
        deal();
    }
}
