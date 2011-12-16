#include "Pipe.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <vector>
#include <iostream>



Semaphore::Semaphore(int val)
{
    this->semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    semctl(this->semid, 0, SETVAL, val);
}

Semaphore::~Semaphore()
{
    semctl( this->semid, 1, IPC_RMID );
}

void Semaphore::operate(int op)
{
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = op;
    sop.sem_flg = 0;
    semop(this->semid, &sop, 1);
}

Pipe::Pipe(unsigned int bufsize):size(bufsize), sem_elem(0), sem_empty(bufsize), sem_rlock(1), sem_wlock(1)
{
    data.assign(this->size,0);
    this->front = this->rear = 0;
}

void Pipe::write(const std::vector<PIPE_DATA> & data)
{
    assert(data.size() <= this->size && "Not implemented for this case.");
    sem_empty.operate(-(int)data.size());
    sem_wlock.operate(-1);
    std::cout << "Write:" << std::endl;
    for(int i = 0; i < data.size(); i++)
    {
        this->rear = (this->rear + 1) % this->size;
        this->data[this->rear] = data[i];
        std::cout << data[i] << " " << std::endl;
    }
    
    sem_wlock.operate(1);
    sem_elem.operate(data.size());
}

void Pipe::read(std::vector<PIPE_DATA> & data, int n)
{
    assert(n <= this->size && "Not implemented for this case.");
    sem_elem.operate(-(int)n);
    sem_rlock.operate(-1);
    data.clear();
    std::cout << "Read: " << std::endl;
    for(int i = 0; i < n; i++)
    {
        this->front = (this->front + 1) % this->size;
        data.push_back(this->data[this->front]);
        std::cout << this->data[this->front] << " " << std::endl;
    }
    
    sem_rlock.operate(1);
    sem_empty.operate(n);
}
    
/*
int main()
{
    Pipe p(1000);

    std::vector<PIPE_DATA> a;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    a.push_back(4);
    a.push_back(5);
    p.write(a);


    std::vector<PIPE_DATA> b;
    p.read(b, 2);

    for(int i=0; i < b.size(); i++)
    {
        std::cout << b[i] << " ";
    }
    p.write(a);
    p.read(b, 7);

    for(int i=0; i < b.size(); i++)
    {
        std::cout << b[i] << " ";
    }
}*/
