#ifndef AVS_API_ENCODEPEWORKER_H
#define AVS_API_ENCODEPEWORKER_H

#include "EncodeWorker.h"

// typedef struct _tagInsrInfo
// {
//     uint32_t n;
//     uint32_t med;
//     uint32_t left;
//     uint32_t right;
// }InsrInfo;

// typedef struct _tagPEAlign
// {
//     uint8_t rev[2];
//     int mis[2];
//     uint32_t seqlen[2];
//     uint64_t alignpos[2];
// }PEAlign;

class EncodePEWorker:public EncodeWorker
{
public:
    EncodePEWorker(int num, IRef* refptr);
    ~EncodePEWorker();
    virtual void doTask(MemBuf *ptr);
    void getBlockRead(MemBuf *ptr);
    void addBlockPEInfo();
private:
    BlockInfoPE m_info;
};

#endif
