#include "SeqLenProcess.h"
#include "IWorker.h"


SeqLenProcess::SeqLenProcess(IWorker *ptr)
{
    m_seqlenElPtr = ptr->getseqLenElPtr();
    m_profile = ptr->m_profile;
}

SeqLenProcess::~SeqLenProcess()
{
}

int SeqLenProcess::compress(char *outptr) //开发者需要实现的逻辑
{
    // uint64_t destlen = m_seqlenElPtr->m_LenArry.size()*4;
    uint64_t destlen = compressBound(m_seqlenElPtr->m_LenArry.size()*4);

    uint8_t *psrc = (uint8_t*)m_seqlenElPtr->m_LenArry.getdata();

    int ret = compress2((uint8_t*)outptr, &destlen, reinterpret_cast<const uint8_t*>(psrc), m_seqlenElPtr->m_LenArry.size()*4, Z_DEFAULT_COMPRESSION);
    if(ret != Z_OK)
    {
        arc_stdout(level::ERROR, "SeqLenProcess compress error");
        return 0;
    }
    return destlen;
}


int SeqLenProcess::decompress(int inlen, char *inptr) //开发者需要实现的逻辑
{
    uint64_t destlen = m_seqlenElPtr->m_LenArry.capacity()*4;
    uint8_t *pdest = (uint8_t*)m_seqlenElPtr->m_LenArry.getdata();
    int ret = uncompress(pdest, &destlen, (uint8_t*)inptr, inlen);
    if(ret != Z_OK)
    {
        arc_stdout(level::ERROR, "SeqLenProcess decompress error");
    }
    m_seqlenElPtr->m_LenArry.updatesize(destlen/4);
    return destlen;
}
