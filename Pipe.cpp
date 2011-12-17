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

bool Semaphore::try_operate(int op)
{
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = op;
    sop.sem_flg = IPC_NOWAIT;
    if (semop(this->semid, &sop, 1) < 0)
        return false;
    else
        return true;
}


void Pipe::write(const std::vector<PIPE_DATA> & data)
{
    sem_lock.operate(-1);
    std::cout << "Write:" << std::endl;
    for(int i = 0; i < data.size(); i++)
    {
        this->data.push(data[i]);
        std::cout << data[i] << " " << std::endl;
    }

    sem_lock.operate(1);
    sem_elem.operate(data.size());
}

void Pipe::read(std::vector<PIPE_DATA> & data, int n)
{
    sem_elem.operate(-(int)n);
    sem_lock.operate(-1);
    data.clear();
    std::cout << "Read: " << std::endl;
    for(int i = 0; i < n; i++)
    {
        data.push_back(this->data.front());

        std::cout << this->data.front() << " " << std::endl;
        this->data.pop();
    }      
    sem_lock.operate(1);
}

bool Pipe::try_read(std::vector<PIPE_DATA> & data, int n)
{
    if (sem_elem.try_operate(-(int)n) == false)
    {
        return false;
    }
    sem_lock.operate(-1);
    data.clear();
    std::cout << "Read: " << std::endl;
    for(int i = 0; i < n; i++)
    {
        data.push_back(this->data.front());

        std::cout << this->data.front() << " " << std::endl;
        this->data.pop();
    } 
    sem_lock.operate(1);
    return true;
}

bool Pipe::peek(int * result)
{
    sem_lock.operate(-1);
    bool retval;
    if (!this->data.empty())
    {
        *result = this->data.front();
        retval = true;
    }
    else
    {
        retval = false;
    }
    sem_lock.operate(1);
    return retval;
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
