#ifndef AVS_API_SEQPROCESS_H
#define AVS_API_SEQPROCESS_H

#include "IProcess.h"
#include "Encap.h"
#include "IRef.h"

class IWorker;
class SeqElement;
class SeqLenElement;
class QualEmement;

class SeqProcess:public IProcess
{
public:
    SeqProcess(IWorker *ptr);
    ~SeqProcess();
    virtual int compress(char *outptr);
    virtual int decompress(int inlen, char *inptr);
    
private:
    SeqElement *m_seqElPtr = nullptr;
    SeqLenElement *m_seqLenElPtr = nullptr;
    QualEmement *m_qualElPtr = nullptr;
    std::shared_ptr<Module> m_profile;
};




#endif //AVS_API_SEQPROCESS_H