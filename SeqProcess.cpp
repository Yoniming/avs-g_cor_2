#include "SeqProcess.h"
#include "IWorker.h"

const int SLEVEL = 3;

SeqProcess::SeqProcess(IWorker *ptr)
{
    m_seqElPtr = ptr->getseqElPtr();
    m_seqLenElPtr = ptr->getseqLenElPtr();
    m_qualElPtr = ptr->getqualElPtr();
    m_profile = ptr->m_profile;

}

SeqProcess::~SeqProcess()
{
}

int SeqProcess::compress(char *outptr)
{
    auto compress = m_profile->getCompress();
    auto timer1 = m_profile->getTimer();
    timer1->start("SeqTime");

    uint64_t destlen = m_seqElPtr->m_rawBuf.size();
    uint8_t *psrc = (uint8_t*)m_seqElPtr->m_rawBuf.getdata();
    int ret = compress2((uint8_t*)outptr, &destlen, psrc, m_seqElPtr->m_rawBuf.size(), Z_DEFAULT_COMPRESSION);

    int rawsize = m_seqElPtr->m_rawBuf.size()+m_seqLenElPtr->m_LenArry.size();
    compress->recordCompress("seq", rawsize, destlen);
    timer1->stop();
    if(ret != Z_OK)
    {
        arc_stdout(level::ERROR, "SeqProcess compress error");
        return 0;
    }
    return destlen;
}

int SeqProcess::decompress(int inlen, char *inptr)
{
    uint64_t destlen = m_seqElPtr->m_rawBuf.capacity();
    uint8_t *pdest = (uint8_t*)m_seqElPtr->m_rawBuf.getdata();
    int ret = uncompress(pdest, &destlen, (uint8_t*)inptr, inlen);
    if(ret != Z_OK)
    {
        arc_stdout(level::ERROR, "SeqProcess decompress error");
    }
    m_seqElPtr->m_rawBuf.updatesize(destlen);
    return destlen;
}
