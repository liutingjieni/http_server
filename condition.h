/*************************************************************************
	> File Name: condition.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年07月27日 星期一 19时29分55秒
 ************************************************************************/


class Condition {
public:
    explicit Condition(MutexLock &mutex)
        : mutex_(mutex)
    {
        pthread_cond_init(&pcond_, NULL);
    }

    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }
    void wait()
    {
        pthread_cond_wait(&pcond_, mutex_.get_mutex());
    }

    void notify()
    {
        pthread_cond_signal(&pcond_);
    }

    void notify_all()
    {
        pthread_cond_broadcast(&pcond_);
    }

private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;
};
