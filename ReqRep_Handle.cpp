#include "ReqRep.h"
#include "JSON.h"
#include <vector>
#include <string>
#include <stdlib.h>
#include <string.h>
#include "Pipe.h"
#include <pthread.h>
#include <iostream>

void ResultBroadcast(int result);

Pipe repo;

pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

int g_producer_cnt = 0;
int g_producer_fin = 0;
std::vector<TCPSocket*> g_producer_sock;

int g_consumer_cnt = 0;
int g_consumer_hang = 0;


void ResultBroadcast(int result)
{
    std::cout << "broadcast " << result << std::endl;
    std::vector<int> vec_result;
    vec_result.push_back(result);
    
    ReplyGet reply(vec_result);
    std::string message = reply.Serialize();
    
    pthread_mutex_lock(&g_lock);
    for(int i = 0; i < g_producer_sock.size(); i++)
    {
        g_producer_sock[i]->sendString(message);
    }
    pthread_mutex_unlock(&g_lock);
}

void RequestPut::Handle(TCPSocket & sock) const
{
    repo.write(this->data);
    ReportStatus s("ok");
    sock.sendString(s.Serialize());
}


void RequestGet::Handle(TCPSocket & sock) const
{
    std::vector<int> integers;
    
    pthread_mutex_lock(&g_lock);
    g_consumer_hang++;
    bool all_hang = (g_consumer_hang == g_consumer_cnt);
    pthread_mutex_unlock(&g_lock);
    
    if (all_hang)
    {
        if( repo.try_read(integers, this->num) == false )
        {
            int result = -1;
            if (repo.peek(&result))
            {
                ResultBroadcast(result);
                std::cout << "Consumer finish" << std::endl;
                return;
            }
            else
            {
                repo.read(integers, this->num);
            }
        }
    }
    else
    {
        repo.read(integers, this->num);
    }
    
    pthread_mutex_lock(&g_lock);
    g_consumer_hang--;
    pthread_mutex_unlock(&g_lock);
    
    ReplyGet rep(integers);
    sock.sendString(rep.Serialize());
}


void ReportStatus::Handle(TCPSocket & sock) const
{
    std::cout << this->status << std::endl;
    if(this->status == "producer-start")
    {
        pthread_mutex_lock(&g_lock);
        g_producer_sock.push_back(&sock);
        g_producer_cnt++;
        pthread_mutex_unlock(&g_lock);
    }
    else if(this->status == "producer-finish")
    {
        pthread_mutex_lock(&g_lock);
        g_producer_fin++;
        pthread_mutex_unlock(&g_lock);
    }
    else if(this->status == "consumer-start")
    {
        pthread_mutex_lock(&g_lock);
        g_consumer_cnt++;
        pthread_mutex_unlock(&g_lock);
    }
    
    ReportStatus s("ok");
    sock.sendString(s.Serialize());
}

void Request::HandleRequest(TCPSocket & sock)
{
    const int RCVBUFSIZE = 32;
    std::string s;

    char buffer[RCVBUFSIZE];
    int recvSize;
    while (s.find('}') == std::string::npos && ( (recvSize = sock.recv(buffer, RCVBUFSIZE - 1)) > 0)) 
    {
        buffer[recvSize] = 0;
        s += buffer;
    }
    
    RequestGet reqg;
    RequestPut reqp;
    ReportStatus reps;
    Request * reqs[] = {&reqg, &reqp, &reps};

    for(int i=0; i < 3; i++)
    {
        try
        {
            reqs[i]->Deserialize(s);
            reqs[i]->Handle(sock);
            return;
        }
        catch (SerializationError&)
        {
            continue;
        }
    }
    throw SerializationError();
}
