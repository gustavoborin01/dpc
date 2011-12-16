#include "ReqRep.h"
#include "JSON.h"
#include <vector>
#include <string>
#include <stdlib.h>
#include <string.h>
#include "Pipe.h"

#include <iostream>
Pipe repo(1000);

std::string ReplyGet::Serialize() const
{
    cJSON *root = cJSON_CreateObject();
    cJSON *arr = cJSON_CreateIntArray(&integers[0], integers.size());
    cJSON_AddItemToObject(root, "integers", arr);

    char * out = cJSON_Print(root);
    std::string result = out;
    cJSON_Delete(root);
    free(out);
    return result;
}

void ReplyGet::Deserialize(const std::string & s)
{
    cJSON * root = cJSON_Parse(s.c_str());
    if (root != NULL)
    {
        cJSON * arr = cJSON_GetObjectItem(root, "integers");
        if(arr != NULL)
        {
            this->integers.clear();
            for(int i = 0; i < cJSON_GetArraySize(arr); i++)
            {
                cJSON * item = cJSON_GetArrayItem(arr, i);
                this->integers.push_back(item->valueint);
            }
        }
        else
        {
            cJSON_Delete(root);
            throw SerializationError();
        }
    }
    else
    {
        cJSON_Delete(root);
        throw SerializationError();
    }
    cJSON_Delete(root);
}

RequestPut::RequestPut(const std::vector<int> & data)
{
    this->data = data;
}

std::string RequestPut::Serialize() const
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "action", cJSON_CreateString("put"));
    cJSON *arr = cJSON_CreateIntArray(&data[0], data.size());
    cJSON_AddItemToObject(root, "integers", arr);

    char * out = cJSON_Print(root);
    std::string result = out;
    cJSON_Delete(root);
    free(out);
    return result;
}

void RequestPut::Deserialize(const std::string & s)
{
    cJSON * root = cJSON_Parse(s.c_str());
    if (root != NULL)
    {
        cJSON * act = cJSON_GetObjectItem(root, "action");
        if (act == NULL || strcmp(act->valuestring, "put")){
            cJSON_Delete(root);
            throw SerializationError();
        }
        cJSON * arr = cJSON_GetObjectItem(root, "integers");
        if(arr != NULL)
        {
            this->data.clear();
            for(int i = 0; i < cJSON_GetArraySize(arr); i++)
            {
                cJSON * item = cJSON_GetArrayItem(arr, i);
                this->data.push_back(item->valueint);
            }
        }
        else
        {
            cJSON_Delete(root);
            throw SerializationError();
        }
    }
    else
    {
        cJSON_Delete(root);
        throw SerializationError();
    }
    cJSON_Delete(root);
}

void RequestPut::Handle(TCPSocket &) const
{
    std::cout << "Hello Put" << std::endl;
    repo.write(this->data);
    
}


RequestGet::RequestGet(int num)
{
    this->num = num;
}

std::string RequestGet::Serialize() const
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "action", cJSON_CreateString("get"));
    cJSON_AddItemToObject(root, "num", cJSON_CreateNumber(this->num));

    char * out = cJSON_Print(root);
    std::string result = out;
    cJSON_Delete(root);
    free(out);
    return result;
}

void RequestGet::Deserialize(const std::string & s)
{
    cJSON * root = cJSON_Parse(s.c_str());
    if (root != NULL)
    {
        cJSON * act = cJSON_GetObjectItem(root, "action");
        if (act == NULL || strcmp(act->valuestring, "get")){
            cJSON_Delete(root);
            throw SerializationError();
        }
        cJSON * item = cJSON_GetObjectItem(root, "num");
        if(item != NULL)
        {
            this->num = item->valueint;
        }
        else
        {
            cJSON_Delete(root);
            throw SerializationError();
        }
    }
    else
    {
        cJSON_Delete(root);
        throw SerializationError();
    }
    cJSON_Delete(root);
}

void RequestGet::Handle(TCPSocket & sock) const
{
    std::vector<int> integers;
    repo.read(integers, this->num);
    
    ReplyGet rep(integers);
    sock.sendString(rep.Serialize());
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
    Request * reqs[] = {&reqg, &reqp};

    for(int i=0; i < 2; i++)
    {
        try
        {
            std::cout << i << std::endl;
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
