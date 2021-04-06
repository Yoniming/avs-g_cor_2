#ifndef AVS_API_NAMEPROCESS_H
#define AVS_API_NAMEPROCESS_H

#include "IProcess.h"
#include <vector>
#include <set>

class IWorker;
class NameElement;
class NameLenElement;

class NameProcess:public IProcess
{
public:
    NameProcess(IWorker *ptr);
    ~NameProcess();
    virtual int compress(char *outptr);
    virtual int decompress(int inlen, char *inptr);
 
private:
    NameElement *m_nameElPtr = nullptr;
    NameLenElement *m_nameLenElPtr = nullptr;
    std::shared_ptr<Module> m_profile;
};


#endif