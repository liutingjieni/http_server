/*************************************************************************
	> File Name: timer.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年10月20日 星期二 17时09分20秒
 ************************************************************************/

#include <iostream>
#include <sys/timerfd.h>

class Timer {
public:
    Timer()
    {
        // 用来创建一个定时器描述符, 
        // CLOCK_MONOTONIC:以固定的速率运行, 从不进行调整和复位, 
        // 不受任何系统time-of-day时钟修改的影响
        timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
        if (timerfd < 0) {
            printf("timerfd_create() error\n");
        }

        struct timespec nw;
        if (clock_gettime(CLOCK_MONOTONIC, &nw) != 0) {
            printf("clock_gettime() error\n");
        }
        
        //it_value是首次超时时间, 需要填写从clock_gettime获取的时间 + 要超时的时间
        its.it_value.tv_sec = nw.tv_sec + interval;
        its.it_value.tv_nsec = 0;
        
        //it_interval是后续周期性超时时间
        its.it_interval.tv_sec = interval;
        its.it_interval.tv_nsec = 0;

        //用于启动和停止定时器
        //第一个参数为timerfd_create获得的定时器文件描述符
        //第二个参数为0表示相对定时器, 为TFD_TIMER_ABSTIME表示绝对定时器
        if (timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &its, NULL) != 0) {
            printf("timerfd_settime() error\n");
        }
        printf("strat timer\n");
    }

    int get_fd() const
    {
        return timerfd;
    }

    void reset()
    {
        its.it_value.tv_sec = its.it_value.tv_sec + its.it_interval.tv_sec;
        if (timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &its, NULL) != 0) {
            printf("timerfd_settime() error\n");
        }
    }

private:
    int interval = 1; //一次心跳时间
    struct itimerspec its; 
    int timerfd;
};
