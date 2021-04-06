#ifndef AVS_API_ENCODEWORKER_H
#define AVS_API_ENCODEWORKER_H

#include "IWorker.h"
#include "BufPool.h"
#include <fstream>

class EncodeWorker:public IWorker
{
public:
    EncodeWorker(int num, IRef* refptr);
    virtual ~EncodeWorker();
    virtual void doJob();
    virtual void doTask(MemBuf *ptr) = 0;
    int DoCompressData();
    int DoCompressPart(IProcess *ptr, int id, char *outptr);
protected:
    MemBufPool *m_memBufPoolPtr = nullptr;
    Memory<char> m_outbuf;
};




#endif //AVS_API_ENCODEWORKER_H