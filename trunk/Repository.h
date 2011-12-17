#ifndef REPO_H
#define REPO_H
#include "Pipe.h"

class Repository: public Pipe
{
    
private:
    Repository() : lock(1) { };
    Repository(const Repository &){};
    static Repository * instance;
    Semaphore lock;
    
};
#endif
