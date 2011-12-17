#include "request.h"
#include "json.h"
#include <vector>
#include <string>
#include <stdlib.h>
#include <string.h>

PutRequest::PutRequest(const std::vector<int> & data)
{
    this->data = data;
}

std::string PutRequest::Serialize() const
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

void PutRequest::Deserialize(const std::string & s)
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

GetRequest::GetRequest(int num)
{
    this->num = num;
}

std::string GetRequest::Serialize() const
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

void GetRequest::Deserialize(const std::string & s)
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


