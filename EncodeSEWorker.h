#ifndef AVS_API_ENCODESEWORKER_H
#define AVS_API_ENCODESEWORKER_H

#include "EncodeWorker.h"

class EncodeSEWorker:public EncodeWorker
{
public:
    EncodeSEWorker(int num, IRef* refptr);
    ~EncodeSEWorker();
    virtual void doTask(MemBuf *ptr);
    void getBlockRead(MemBuf *ptr);
    void doAlign();
    void addBlockSEInfo();
private:
    BlockInfoSE m_info;
};


#endif