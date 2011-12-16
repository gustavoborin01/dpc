#include "Socket.h"
#include "ThreadPool.h"
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <string.h>
#include <string>
#include <unistd.h>

#include "ReqRep.h"


using namespace std;

class AnswerWorker : public WorkerThread
{
public:
    virtual void execute()
	{
        try {
            sock->keepAlive();
            for(;;)
            {
                Request::HandleRequest(*sock);
            }
        }
        catch (...) {
            
        }
	}

    AnswerWorker(TCPSocket * sock) : sock(sock) {}
    
    virtual ~AnswerWorker(){delete sock;}
private:
    TCPSocket * sock;
    AnswerWorker(const AnswerWorker&){};
};


int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <port>" << endl;
        exit(1);
    }

    unsigned short port = atoi(argv[1]);
    ThreadPool pool(100);
    
    try {
        TCPServerSocket servSock(port);
        for (;;) {
            try {
                // Waiting for connection
                TCPSocket *clntSock = servSock.accept();
                AnswerWorker * worker = new AnswerWorker(clntSock);
                pool.pushWork(worker);
            } catch (exception &e) {
                cerr << e.what() << endl;
            }
        }
    }
    catch (exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
