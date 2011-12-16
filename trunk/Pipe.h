#ifndef PIPE_H
#define PIPE_H

#include <vector>

//Define data types
#define PIPE_DATA int

class Semaphore
{
public:
    Semaphore(int val=0);
    ~Semaphore();
    void operate(int op);
private:
    int semid;
    Semaphore(const Semaphore &){};
};



class Pipe
{
public:
    Pipe(unsigned int bufsize);
    void write(const std::vector<PIPE_DATA> & data);
    
    void read(std::vector<PIPE_DATA> & data, int n);
    
private:
    unsigned int front, rear;
    unsigned int size;
    std::vector<PIPE_DATA> data;
    Semaphore sem_elem;
    Semaphore sem_empty;
    Semaphore sem_rlock;
    Semaphore sem_wlock;
};
#endif
