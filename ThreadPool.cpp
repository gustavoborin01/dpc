#include <stdlib.h>
#include "ThreadPool.h"

using namespace std;


ThreadPool::ThreadPool(int maxThreads) {
    if (maxThreads < 1) 
        maxThreads=1;
  
    mutex = PTHREAD_MUTEX_INITIALIZER;
    
    mutexWork = PTHREAD_MUTEX_INITIALIZER; 
    condWork = PTHREAD_COND_INITIALIZER;

    incompleteWork = 0;
    sem_init(&availableWork, 0, 0);
    
    for(int i = 0; i<maxThreads; ++i) {
        pthread_t thread;
        pthread_create(&thread, NULL, ThreadMain, (void *) this ); 
    }
}



ThreadPool::~ThreadPool() {
    pthread_mutex_lock(&mutexWork);
	while( incompleteWork>0 ) {
		pthread_cond_wait(&condWork, &mutexWork);
	}
	pthread_mutex_unlock(&mutexWork);
	
	sem_destroy(&availableWork);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexWork);
}

void ThreadPool::pushWork(WorkerThread *workerThread) {
    pthread_mutex_lock(&mutexWork);
	incompleteWork++;
	pthread_mutex_unlock(&mutexWork);
    
	pthread_mutex_lock(&mutex);
	workerQueue.push(workerThread);
	pthread_mutex_unlock(&mutex);

	sem_post(&availableWork);
}

WorkerThread * ThreadPool::popWork() {
	sem_wait(&availableWork);
	
	pthread_mutex_lock(&mutex);
	if(workerQueue.empty())
	    return NULL;
	WorkerThread * worker = workerQueue.front();
    workerQueue.pop();
	pthread_mutex_unlock(&mutex);
	
    return worker;
}

void * ThreadMain(void *param) {
	WorkerThread *worker = NULL;
    ThreadPool * pool = (ThreadPool *)param;
	while( worker = pool->popWork()) {
    	worker->execute();
        delete worker;

		pthread_mutex_lock( &(pool->mutexWork) );
	 	pool->incompleteWork--;
	 	if(pool->incompleteWork == 0) {
	 	    pthread_cond_broadcast(&(pool->condWork));
	 	}
		pthread_mutex_unlock( &(pool->mutexWork) );
	}
	return 0;
}
