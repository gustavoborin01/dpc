#ifndef PIPE_H
#define PIPE_H

#include <queue>

//Define data types
#define PIPE_DATA int

class Semaphore
{
public:
    Semaphore(int val=0);
    ~Semaphore();
    void operate(int op);
    bool try_operate(int op);
private:
    int semid;
    Semaphore(const Semaphore &){};
};


class Pipe
{
public:
    Pipe() : sem_elem(0), sem_lock(1) { };
    
    void write(const std::vector<PIPE_DATA> & data);
    void read(std::vector<PIPE_DATA> & data, int n);
    bool try_read(std::vector<PIPE_DATA> & data, int n);
    bool peek(int * result);
    
private:

    std::queue<PIPE_DATA> data;
    Semaphore sem_elem;
    Semaphore sem_lock;
};
#endif
