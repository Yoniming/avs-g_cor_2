#include "DecodePipeWorker.h"

uint64_t DecodePipeWorker::m_Totallen = 0;
std::mutex DecodePipeWorker::m_mtx_pipe;

DecodePipeWorker::DecodePipeWorker(int num,  IRef* refptr):
    DecodeWorker(num, refptr)
{
    m_lockPtr = new std::unique_lock<std::mutex>(m_mtx_pipe, std::defer_lock);
    m_outptr1 = m_inbuf.getdata();
    m_outptr2 = m_inbuf.getdata() + m_paramPtr->m_iBlockSize/2;
}

DecodePipeWorker::~DecodePipeWorker()
{
    RELEASEPTR(m_lockPtr);
}

uint32_t DecodePipeWorker::recoverDataNoComment()
{
    char *outptr1 = m_outptr1;
    char *outptr2 = m_outptr2;
    int len = 0;
    int count = m_seqLenElPtr->m_LenArry.size();
    switch (m_paramPtr->m_PipeOut)
    {
    case PIPEOUT_SE:
    case PIPEOUT_PE:
    {
        for(uint32_t i=0;i<count;i++)
        {
            len = copyReadDataNoComment(i, outptr1);
            outptr1 += len;
        }
        return outptr1-m_outptr1;
    }
    case PIPEOUT_PE_1:
    {
        for(uint32_t i=0;i<count;i++)
        {
            if (!(i & 1)) //第一条read
            {
                len = copyReadDataNoComment(i, outptr1);
                outptr1 += len;
            }
            else
            {
                m_nameElPtr->m_bufPtr += m_namelenElPtr->m_LenArry[i];
                m_seqElPtr->m_bufPtr += m_seqLenElPtr->m_LenArry[i];
                m_qualElPtr->m_bufPtr += m_seqLenElPtr->m_LenArry[i];
            }
        }
        return outptr1 - m_outptr1;
    }
    case PIPEOUT_PE_2:
    {
        for(uint32_t i=0;i<count;i++)
        {
            if (i & 1) //第二条read
            {
                len = copyReadDataNoComment(i, outptr2);
                outptr2 += len;
            } 
            else
            {
                m_nameElPtr->m_bufPtr += m_namelenElPtr->m_LenArry[i];
                m_seqElPtr->m_bufPtr += m_seqLenElPtr->m_LenArry[i];
                m_qualElPtr->m_bufPtr += m_seqLenElPtr->m_LenArry[i];
            }   
        }
        return outptr2 - m_outptr2;
    }
    default:
        break;
    }
    return 0;
}

uint32_t DecodePipeWorker::recoverDataWithComment()
{
    char *outptr1 = m_outptr1;
    char *outptr2 = m_outptr2;
    int len = 0;
    int count = m_seqLenElPtr->m_LenArry.size();
    switch (m_paramPtr->m_PipeOut)
    {
    case PIPEOUT_SE:
    case PIPEOUT_PE:
    {
        for(uint32_t i=0;i<count;i++)
        {
            len = copyReadDataWithComment(i, outptr1);
            outptr1 += len;
        }
        return outptr1-m_outptr1;
    }
    case PIPEOUT_PE_1:
    {
        for(uint32_t i=0;i<count;i++)
        {
            if (!(i & 1)) //第一条read
            {
                len = copyReadDataWithComment(i, outptr1);
                outptr1 += len;
            }
            else
            {
                m_nameElPtr->m_bufPtr += m_namelenElPtr->m_LenArry[i];
                m_seqElPtr->m_bufPtr += m_seqLenElPtr->m_LenArry[i];
                m_qualElPtr->m_bufPtr += m_seqLenElPtr->m_LenArry[i];
            }
        }
        return outptr1 - m_outptr1;
    }
    case PIPEOUT_PE_2:
    {
        for(uint32_t i=0;i<count;i++)
        {
            if (i & 1) //第二条read
            {
                len = copyReadDataWithComment(i, outptr2);
                outptr2 += len;
            } 
            else
            {
                m_nameElPtr->m_bufPtr += m_namelenElPtr->m_LenArry[i];
                m_seqElPtr->m_bufPtr += m_seqLenElPtr->m_LenArry[i];
                m_qualElPtr->m_bufPtr += m_seqLenElPtr->m_LenArry[i];
            }   
        }
        return outptr2 - m_outptr2;
    }
    default:
        break;
    }
    return 0;
}

void DecodePipeWorker::outPutData(uint32_t len)
{
    switch (m_paramPtr->m_PipeOut)
    {
    case PIPEOUT_SE:
    case PIPEOUT_PE_1:
        pipeOutWrite(m_outptr1, len, m_blocksize[0], m_blocksize[0], m_blockoffset[0]);
        break;
    case PIPEOUT_PE_2:
        pipeOutWrite(m_outptr2, len, m_blocksize[1], m_blocksize[1], m_blockoffset[1]);
        break;
    case PIPEOUT_PE:
        pipeOutWrite(m_outptr1, len, m_blocksize[0]+m_blocksize[1], m_blocksize[0], m_blockoffset[0]);
        break;
    default:
        break;
    } 
}

void DecodePipeWorker::pipeOutWrite(const char *pbuf, uint32_t len, uint32_t orglen, uint32_t tlen, uint64_t offset)
{
    if(len == orglen)
    {
        while (true)
        {
            m_lockPtr->lock();
            if(offset == m_Totallen)
            {
                m_Totallen += tlen;
                fwrite(pbuf, len, 1, stdout);
                fflush(stdout);
                m_lockPtr->unlock();
                break;
            }
            m_lockPtr->unlock();
        }
    }
    else
    {
        arc_stdout(level::ERROR, "decode len %ld is not equal encode len %ld", len, orglen);
        exit(1);
    }
}
