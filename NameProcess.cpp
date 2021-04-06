#include "NameProcess.h"
#include "IWorker.h"


NameProcess::NameProcess(IWorker *ptr)
{
    m_nameElPtr = ptr->getnameElPtr();
    m_nameLenElPtr = ptr->getnamelenElPtr();
    m_profile = ptr->m_profile;
}

NameProcess::~NameProcess()
{
}

int NameProcess::compress(char *outptr)
{
    auto compress = m_profile->getCompress();
    auto timer1 = m_profile->getTimer();
    timer1->start("NameTime");

    uint64_t destlen = m_nameElPtr->m_rawBuf.size();
    uint8_t *psrc = (uint8_t*)m_nameElPtr->m_rawBuf.getdata();
    int ret = compress2((uint8_t*)outptr, &destlen, psrc, m_nameElPtr->m_rawBuf.size(), Z_DEFAULT_COMPRESSION);
    
    int rawsize = m_nameElPtr->m_rawBuf.size()+m_nameLenElPtr->m_LenArry.size()*2;
    compress->recordCompress("name", rawsize, destlen);
    timer1->stop();
    if(ret != Z_OK)
    {
        arc_stdout(level::ERROR, "NameProcess compress error");
        return 0;
    }

    return destlen;
}

int NameProcess::decompress(int inlen, char *inptr)
{
    uint64_t destlen = m_nameElPtr->m_rawBuf.capacity();
    uint8_t *pdest = (uint8_t*)m_nameElPtr->m_rawBuf.getdata();
    int ret = uncompress(pdest, &destlen, (uint8_t*)inptr, inlen);
    if(ret != Z_OK)
    {
        arc_stdout(level::ERROR, "NameProcess decompress error");
    }
    m_nameElPtr->m_rawBuf.updatesize(destlen);
    return destlen;
}
