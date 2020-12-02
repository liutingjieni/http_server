/*************************************************************************
	> File Name: threadpool.cpp
	> Author:jieni 
	> Mail: 
	> Created Time: 2020年07月24日 星期五 16时31分36秒
 ************************************************************************/
#include <list>
#include <functional>
#include "mutex.h"
#include "condition.h"
#include "socket.h"
typedef std::function<void(std::shared_ptr<conn>)> callback;

typedef struct task {
    callback callback_;
    std::shared_ptr<conn> conn_;
}Task;

class Threadpool {
public:
    Threadpool();
    ~Threadpool();
    bool push_back(Task);
    static void *worker(void *arg);
    void run();

private:
    int numthreads;
    int max_requests;
    pthread_t *threads;
    std::list<Task> workqueue;
    MutexLock mutex;
    Condition cond_;
};

Threadpool::Threadpool() 
    : numthreads(8), 
      cond_(mutex),
      max_requests(100)
{
    if ((numthreads <= 0) || (max_requests <= 0)) {

    }
    threads = new pthread_t[numthreads];
    if (!threads) {

    }
    for(int i = 0; i < numthreads; i++) {
        if ((pthread_create(threads + i, NULL, worker, this)) != 0) {
            delete []threads;

        }
        if (pthread_detach(threads[i])) {
            delete []threads;
        
        }
    }
}

Threadpool::~Threadpool()
{
    delete [] threads;
}

bool Threadpool::push_back(Task task_)
{
    
    {
    MutexLockGuard lock(mutex);
    if (workqueue.size() > max_requests) 
        return false;
    workqueue.push_back(task_);
    }
    cond_.notify();
    return true;
}

void *Threadpool::worker(void *arg) 
{
    Threadpool* pool = (Threadpool *)arg;
    pool->run();
    return pool;
}

void Threadpool::run()
{
    while(1) {
        mutex.lock();        
        while (workqueue.size() < 1) {
            cond_.wait();
        }
        Task task_ = workqueue.front();
        workqueue.pop_front();
        mutex.unlock();
        if(task_.callback_) {
            task_.callback_(task_.conn_);
        }
    }
}
