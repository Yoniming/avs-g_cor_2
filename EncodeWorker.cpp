#include "EncodeWorker.h"

EncodeWorker::EncodeWorker(int num, IRef* refptr):
    IWorker(num, refptr)
{
    m_memBufPoolPtr = m_paramPtr->m_memBufPoolPtr;

    m_outbuf.mem_malloc(m_paramPtr->m_iBlockSize/2);

}

EncodeWorker::~EncodeWorker()
{

}

void EncodeWorker::doJob()
{
    while (true)
    {
        MemBuf *ptr = m_memBufPoolPtr->getFullbuf();
        if(ptr)
        {
            if(ptr->m_len[0]==0)
            {
                m_memBufPoolPtr->addEmptybuf(ptr);
                break;
            }
            else
            {
                doTask(ptr);
            } 
        }
        m_processcount++;
    }
}


int EncodeWorker::DoCompressData()
{
    char *outptr = m_outbuf.getdata();
    int idlen = Encap::setID(BLOCK_ENCAP, outptr);
    outptr += idlen;
    char *psize = outptr;
    outptr += SIZENUM4;
    char *ptmp = outptr;

    int sz = DoCompressPart(m_paramProcessPtr, BLOCK_PARAM, outptr);
    outptr += sz;
    sz = DoCompressPart(m_seqlenProcessPtr, BLOCK_SEQLEN, outptr);
    outptr += sz;
    sz = DoCompressPart(m_nameLenProcessPtr, BLOCK_NAMELEN, outptr);
    outptr += sz;
    sz = DoCompressPart(m_nameProcessPtr, BLOCK_NAME, outptr);
    outptr += sz;
    sz = DoCompressPart(m_seqProcessPtr, BLOCK_SEQ, outptr);
    outptr += sz;
    sz = DoCompressPart(m_qualProcessPtr, BLOCK_QUAL, outptr);
    outptr += sz;

    int size = outptr - ptmp;
    Encap::setSize(size, SIZENUM4, psize);
    return size+SIZENUM4+idlen;
}

int EncodeWorker::DoCompressPart(IProcess *ptr, int id, char *outptr)
{
    int idlen = Encap::setID(id, outptr);
    outptr += idlen;
    char *psize = outptr;
    outptr += SIZENUM4;
    int szlen = ptr->compress(outptr);
    Encap::setSize(szlen, SIZENUM4, psize);
    return szlen+SIZENUM4+idlen;
}