#ifndef REQ_H
#define REQ_H
#include <vector>
#include <string>
#include <exception>
#include "Socket.h"

class SerializationError {};

class Reply
{
public:
    virtual ~Reply(){};
    virtual std::string Serialize() const = 0;
    virtual void Deserialize(const std::string & s) = 0;
};

class ReplyGet: public Reply
{
public:
    ReplyGet(){};
    ReplyGet(const std::vector<int> & integers) : integers(integers){};
    virtual std::string Serialize() const;
    virtual void Deserialize(const std::string & s);
private:
    std::vector<int> integers;
};


class Request 
{
public:
    virtual ~Request(){};
    virtual std::string Serialize() const = 0;
    virtual void Deserialize(const std::string & s) = 0;
    virtual void Handle (TCPSocket &)const{};
    static void HandleRequest(TCPSocket & sock);
};

class RequestPut: public Request
{
public:
    RequestPut() {};
    RequestPut( const std::vector<int> & data);
    virtual std::string Serialize() const;
    virtual void Deserialize(const std::string & s);
    std::vector<int> GetData() const { return data; };
    virtual void Handle(TCPSocket &) const;
private:
    std::vector<int> data;
};


class RequestGet: public Request
{
public:
    RequestGet() {};
    RequestGet(int num);
    virtual std::string Serialize() const;
    virtual void Deserialize(const std::string & s);
    
    int GetNum() const { return num; };
    virtual void Handle(TCPSocket &) const;
private:
    int num;
};

class ReportStatus: public Request
{
public:
    ReportStatus() {};
    ReportStatus(std::string status):status(status) { };
    virtual std::string Serialize() const;
    virtual void Deserialize(const std::string & s);
    
    std::string GetStatus() const { return status; };
    virtual void Handle(TCPSocket &) const;
private:
    std::string status;
};

#endif
