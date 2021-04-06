#ifndef AVS_API_SEQLENPROCESS_H
#define AVS_API_SEQLENPROCESS_H

#include "IProcess.h"

class IWorker;
class SeqLenElement;

class SeqLenProcess:public IProcess
{
public:
    SeqLenProcess(IWorker *ptr);
    ~SeqLenProcess();
    virtual int compress(char *outptr);
    virtual int decompress(int inlen, char *inptr);

private:
    SeqLenElement *m_seqlenElPtr = nullptr;
    std::shared_ptr<Module> m_profile;
};



#endif