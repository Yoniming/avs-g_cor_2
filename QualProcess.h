#ifndef AVS_API_QUALPROCESS_H
#define AVS_API_QUALPROCESS_H

#include "IProcess.h"

class IWorker;
class SeqLenElement;
class QualEmement;
class SeqElement;

class QualProcess:public IProcess
{
public:
    QualProcess(IWorker *ptr);
    ~QualProcess();
    virtual int compress(char *outptr);
    virtual int decompress(int inlen, char *inptr);

private:
    QualEmement *m_qualElPtr = nullptr;
    SeqElement *m_seqElPtr = nullptr;
    SeqLenElement *m_seqLenElPtr = nullptr;
    std::shared_ptr<Module> m_profile;
};

#endif