/*************************************************************************
	> File Name: timer.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年10月19日 星期一 18时51分31秒
 ************************************************************************/

#include <iostream>
#include <assert.h>
#include <signal.h>
using namespace std;


#include "epoll.h"
#include "timer_wheel.h"

class timer {
public:
    void set_pipe() {
        int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
        assert(ret != -1);
        setnonblocking(pipefd[1]);

        //Epoll::fd_read(pipefd[0]);
        //Epoll::epoll_add_(pipefd[0]);
    }
    void add_sig(int sig)
    {
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = sig_handler;
        sa.sa_flags |= SA_RESTART;
        sigfillset(&sa.sa_mask);
        assert(sigaction(sig, &sa, NULL) != -1);
    }
    void sig_handler(int sig)
    {
        int save_errno = errno;
        int msg = sig;
        send(pipefd[1], (char *)&msg, 1, 0);
        errno = save_errno;
    }
    void set_sig()
    {
        set_pipe();
        add_sig(SIGALRM);
        add_sig(SIGTERM);
        alarm(TIMESLOT);
    }

private:
    int pipefd[2];
    const int TIMESLOT = 1;
    bool timeout = false;
};





