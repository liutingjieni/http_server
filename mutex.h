/*************************************************************************
	> File Name: mutex.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年07月24日 星期五 16时44分27秒
 ************************************************************************/

#include <pthread.h>

class MutexLock {
public:
    MutexLock() {pthread_mutex_init(&mutex_, NULL); }
    MutexLock(const MutexLock& mutex) 
    {
        mutex_ = mutex.mutex_;
    }
    ~MutexLock() {pthread_mutex_destroy(&mutex_); }
    void lock() 
    {
        pthread_mutex_lock(&mutex_);
    }

    void unlock() 
    {
        pthread_mutex_unlock(&mutex_);
    }
    pthread_mutex_t *get_mutex()
    {
        return &mutex_;
    }
private:
    pthread_mutex_t mutex_;
};

class MutexLockGuard {
public:
    explicit MutexLockGuard(MutexLock& mutex) 
    : mutex_(mutex)
    {
        mutex_.lock();
    }
    ~MutexLockGuard()
    {
        mutex_.unlock();
    }
private:
    MutexLock& mutex_;
};
