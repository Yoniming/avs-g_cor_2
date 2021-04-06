#include "NameLenProcess.h"
#include "IWorker.h"

NameLenProcess::NameLenProcess(IWorker *ptr)
{
    m_nameLenElPtr = ptr->getnamelenElPtr();
}

NameLenProcess::~NameLenProcess()
{
}

int NameLenProcess::compress(char *outptr)
{
    uint64_t destlen = m_nameLenElPtr->m_LenArry.size()*4;
    uint8_t *psrc = (uint8_t*)m_nameLenElPtr->m_LenArry.getdata();
    int ret = compress2((uint8_t*)outptr, &destlen, psrc, m_nameLenElPtr->m_LenArry.size()*4, Z_DEFAULT_COMPRESSION);
    if(ret != Z_OK)
    {
        arc_stdout(level::ERROR, "NameLenProcess compress error");
        return 0;
    }
    return destlen;
}

int NameLenProcess::decompress(int inlen, char *inptr)
{
    uint64_t destlen = m_nameLenElPtr->m_LenArry.capacity()*4;
    uint8_t *pdest = (uint8_t*)m_nameLenElPtr->m_LenArry.getdata();
    int ret = uncompress(pdest, &destlen, (uint8_t*)inptr, inlen);
    if(ret != Z_OK)
    {
        arc_stdout(level::ERROR, "NameLenProcess decompress error");
    }
    m_nameLenElPtr->m_LenArry.updatesize(destlen/4);
    return destlen;
}
