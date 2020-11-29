/*************************************************************************
	> File Name: threadpool.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年07月24日 星期五 16时31分36秒
 ************************************************************************/

#include <iostream>
#include <list>
#include "condition.h"

using namespace std;

template <typename T>
class Threadpool {
public:
    Threadpool(int);
    ~Threadpool();
    bool push_back(T*request);
    static void *worker(void *arg);
    void run();

private:
    int numthreads;
    int max_requests;
    pthread_t *threads;
    list<T *> workqueue;
    MutexLock mutex;
    Condition cond_;
};

template <typename T>
Threadpool<T>::Threadpool(int num) : numthreads(num)
{
    if ((numthreads <= 0) || (max_requests <= 0)) {

    }
    threads = new pthread_t[numthreads];
    if (!threads) {

    }
    for(int i = 0; i < numthreads; i++) {
        cout << "create the " << i <<"th thread" << endl;
        if ((pthread_create(threads + i, NULL, worker, NULL)) != 0) {
            delete []threads;

        }
        if (pthread_detach(threads[i])) {
            delete []threads;
        
        }
    }
}

template <typename T>
Threadpool<T>::~Threadpool()
{
    delete [] threads;
}

template <typename T>
bool Threadpool<T>::push_back(T *request)
{
    
    {
    MutexLockGuard lock(mutex);
    if (workqueue.size() > max_requests) 
        return false;
    workqueue.push_back(request);
    }
    cond_.notify();
    return true;
}

template <typename T>
void *Threadpool<T>::worker(void *arg) 
{
    Threadpool* pool = (Threadpool *)arg;
    pool->run();
    return pool;
}

template <typename T>
void Threadpool<T>::run()
{
    while(1) {
        {
            MutexLockGuard lock(mutex);
            while (workqueue.size() < 1) {
                cond_.wait();
            }

        }
        MutexLock lock(mutex);
        T *request = workqueue.front();
        workqueue.pop_front();
        if(request) {
           request->process(); 
        }
    }
}





