#ifndef AVS_API_DECODEPIPEWORKER_H
#define AVS_API_DECODEPIPEWORKER_H

#include "DecodeWorker.h"

class DecodePipeWorker:public DecodeWorker
{
public:
    DecodePipeWorker(int num, IRef* refptr);
    ~DecodePipeWorker();

    virtual uint32_t recoverDataNoComment();
    virtual uint32_t recoverDataWithComment();
    virtual void outPutData(uint32_t len);

private:
    void pipeOutWrite(const char *pbuf, uint32_t len, uint32_t orglen, uint32_t tlen, uint64_t offset);
private:
    static uint64_t m_Totallen;
    static std::mutex m_mtx_pipe;
    std::unique_lock<std::mutex> *m_lockPtr = nullptr;
    char *m_outptr1 = nullptr;
    char *m_outptr2 = nullptr;
};



#endif //AVS_API_DECODEPIPEWORKER_H