#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <queue>
#include <vector>

class WorkerThread{
public:
    virtual void execute() = 0;

    virtual ~WorkerThread(){}
};


void * ThreadMain(void *param);


class ThreadPool{
public:
    ThreadPool(int maxThreads);
    virtual ~ThreadPool();
    
    void pushWork(WorkerThread *worker);
    
    friend void * ThreadMain(void *param);
private:
    ThreadPool(const ThreadPool&){};
    
    WorkerThread * popWork();

	int incompleteWork;
    pthread_mutex_t mutexWork;
    pthread_cond_t condWork;
        
    sem_t availableWork;

    std::queue<WorkerThread *> workerQueue;
	pthread_mutex_t mutex;
    
};



#endif

