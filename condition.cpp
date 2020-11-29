/*************************************************************************
	> File Name: condition.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年07月27日 星期一 19时29分55秒
 ************************************************************************/

#include <iostream>
#include "mutex.h"
using namespace std;

class Condition {
public:
    explicit Condition(MutexLock &mutex)
        : mutex_(mutex)
    {
        pthread_cond_init(&pond, NULL);
    }

    ~Condition()
    {
        pthread_cond_destory(&pcond);
    }
    void wait()
    {
        pthread_cond_wait(&pcond, mutex_.get)
    }

private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;
};
