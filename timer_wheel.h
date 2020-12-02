/*************************************************************************
	> File Name: Timer.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年10月19日 星期一 15时21分43秒
 ************************************************************************/

#include <iostream>
#include <time.h>
#include <netinet/in.h>
#include <functional>
#include "conn.h"
using namespace::std;

#define BUFFER_SIZE 64
typedef std::function<void(std::shared_ptr<conn>)> time_callback;

class tw_timer {
public:
    tw_timer(int rot, int ts, std::shared_ptr<conn> conn)
    : next(NULL), prev(NULL), rotation(rot), 
      time_slot(ts), conn_(conn) {  }

    void set_time_callback(time_callback cb)
    {
        time_callback_ = cb;
    }

    int rotation;       //记录定时器在时间轮转多少圈后生效
    int time_slot;      //记录定时器属于时间轮上哪个槽
    time_callback time_callback_; //定时器回调函数
    shared_ptr<conn> conn_;    //客户数据
    tw_timer* next;     //下一个定时器
    tw_timer* prev;     //上一个定时器

};

class time_wheel {
public:
    time_wheel() : cur_slot(0)
    {
        for(int i = 0; i < N; i++) {
            slots[i] = NULL; //初始化所有槽
        }
    }
    ~time_wheel()
    {
        //销毁所有槽上所连的链表上的定时器
        for (int i = 0; i < N; i++) {
            tw_timer* tmp = slots[i]; 
            while (tmp) {
                slots[i] = tmp->next;
                delete tmp;
                tmp = slots[i];
            }
        }
    }

    tw_timer* add_timer(shared_ptr<conn>, int timeout); 
    tw_timer* reset_timer(tw_timer* , int ); 
    tw_timer* cur_del_timer(tw_timer* timer);
    void del_timer(shared_ptr<conn> );
    void tick();
    void update_timer(shared_ptr<conn>, int timeout);

private:
    static const int N = 60; //时间轮上槽的数目
    static const int SI = 1; //每一秒时间轮转一次, 即槽间隔为1S
    tw_timer* slots[N];      //
    int cur_slot;            //时间轮的当前槽
    map<shared_ptr<conn>, tw_timer *> get_timer;
};

//根据定时值timeout创建一个定时器, 并把它插入到合适的槽中
tw_timer* time_wheel::add_timer(shared_ptr<conn> conn, int timeout)
{
    if (timeout < 0) {
        return NULL;
    } 
    int ticks = 0;
    //下面根据待插入定时器的超时值计算它将在时间轮转动多少个滴答后被触发,并将滴答数存储与变量ticks
    //如果待插入定时器的超时值小于时间轮的槽间隔SI, 则将ticks向上折合为1
    //否则就将ticks向下折合为timeout/SI
    if (timeout < SI) {
        ticks = 1;
    } 
    else {
        ticks = timeout / SI;
    }

    int rotation = ticks / N; // 计算待插入的定时器在时间轮转动多少圈后被触发
    // 计算待插入的定时器应该插入哪个槽中
    int ts = (cur_slot + (ticks % N)) % N; 
    // 创建新的定时器, 它在时间轮转动rotation后被触发, 且位于ts槽中
    tw_timer* timer = new tw_timer(rotation, ts, conn);
    
    if (!slots[ts]) { //如果该槽中无任何定时器
        slots[ts] = timer;
    }
    else { //头插法将新的定时器插到槽中
        timer->next = slots[ts];
        slots[ts]->prev = timer;
        slots[ts] = timer;
    }

    get_timer[conn] = timer;
    return timer;
}

// 删除目标定时器timer
tw_timer* time_wheel::cur_del_timer(tw_timer* timer)
{
    if (!timer) {
        return nullptr;
    }
    int ts = timer->time_slot;

    //如果要删除的定时器为头结点
    if (timer == slots[ts]) { 
        slots[ts] = slots[ts]->next;
        if (slots[ts]) {
            slots[ts]->prev = NULL;
        }
    }
    else {
        timer->prev->next = timer->next;
        if(timer->next) {
            timer->next->prev = timer->prev;
        }
    }
    return timer;
}
void time_wheel::del_timer(shared_ptr<conn> conn_)
{
    delete cur_del_timer(get_timer[conn_]);
}

tw_timer* time_wheel::reset_timer(tw_timer *timer, int timeout)
{
    timer->prev = timer->next = NULL;
    if (timeout < 0) {
        return NULL;
    } 
    int ticks = 0;
    //下面根据待插入定时器的超时值计算它将在时间轮转动多少个滴答后被触发,并将滴答数存储与变量ticks
    //如果待插入定时器的超时值小于时间轮的槽间隔SI, 则将ticks向上折合为1
    //否则就将ticks向下折合为timeout/SI
    if (timeout < SI) {
        ticks = 1;
    } 
    else {
        ticks = timeout / SI;
    }

    timer->rotation = ticks / N; // 计算待插入的定时器在时间轮转动多少圈后被触发
    // 计算待插入的定时器应该插入哪个槽中
    timer->time_slot = (cur_slot + (ticks % N)) % N; 
    // 创建新的定时器, 它在时间轮转动rotation后被触发, 且位于ts槽中
    
    if (!slots[timer->time_slot]) { //如果该槽中无任何定时器
        slots[timer->time_slot] = timer;
        timer->prev = timer->next = NULL;
    }
    else { //头插法将新的定时器插到槽中
        timer->next = slots[timer->time_slot];
        slots[timer->time_slot]->prev = timer;
        slots[timer->time_slot] = timer;
    }
    
}

void time_wheel::update_timer(shared_ptr<conn> conn, int timeout)
{
    tw_timer* tw(cur_del_timer(get_timer[conn]));
    reset_timer(tw, timeout);
}


//SI时间到后, 调用该函数, 时间轮向前滚动一个槽的间隔
void time_wheel::tick()
{
    //取得时间轮上当前槽的头结点
    tw_timer* tmp = slots[cur_slot];

    while (tmp) { //开始遍历本条链表
        // 如果定时器的rotation > 0, 则它在这一轮连接中不起作用
        if (tmp->rotation > 0) {
            tmp->rotation--;
            tmp = tmp->next;
        }
        //否则, 说明定时器已经到期, 于是执行定时任务, 然后删除该定时器
        else {
            tmp->time_callback_(tmp->conn_);
            //到期的是头结点
            if (tmp == slots[cur_slot]) { 
                slots[cur_slot] = tmp->next;
                delete tmp;
                if (slots[cur_slot]) {
                    slots[cur_slot]->prev = NULL;
                }
                tmp = slots[cur_slot];
            }
            else {
                tmp->prev->next = tmp->next;
                if (tmp->next) {
                    tmp->next->prev = tmp->prev;
                }
                tw_timer* tmp2 = tmp->next;
                delete tmp;
                tmp = tmp2;
            }
        }

    }
    //更新时间轮的当前槽, 以反映时间轮的滚动
    cur_slot = ++cur_slot % N;
}
