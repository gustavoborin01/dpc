#include "ReqRep.h"
#include <iostream>

int main()
{
    std::vector<int> arr;
    for(int i = 0; i < 5; i++)
        arr.push_back(i*123213);
    RequestPut req(arr);
    std::cout << req.Serialize() << std::endl;
    RequestPut req2;
    req2.Deserialize(req.Serialize());
    
    std::cout << req2.Serialize() << std::endl;
    
    
    RequestGet greq(5);
    std::cout << greq.Serialize() << std::endl;
    RequestGet greq2;
    greq2.Deserialize(greq.Serialize());
    
    std::cout << greq2.Serialize() << std::endl;
    
    ReplyGet reqg(arr);
    std::cout << reqg.Serialize() << std::endl;
    ReplyGet reqg2;
    reqg2.Deserialize(reqg.Serialize());
}
