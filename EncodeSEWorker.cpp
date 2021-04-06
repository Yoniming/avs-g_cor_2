#include "EncodeSEWorker.h"


EncodeSEWorker::EncodeSEWorker(int num, IRef* refptr):
    EncodeWorker(num, refptr)
{
}

EncodeSEWorker::~EncodeSEWorker()
{
}

void EncodeSEWorker::doTask(MemBuf *ptr)
{
    auto compress = m_profile->getCompress();
    auto timer1 = m_profile->getTimer();

    memset(&m_info, 0, sizeof(BlockInfoSE));
    m_info.originalSize = ptr->m_len[0];
    m_info.originaloffset = ptr->m_offset[0];
    m_info.fileidx = ptr->m_fileidx;
    
    timer1->start("GetDataTime");
    elementClear();
    
    getBlockRead(ptr);
    timer1->stop();

    timer1->start("TotalCompressTime");
    m_info.compressSize = DoCompressData();
    timer1->stop();

    timer1->start("WriteTime");
    addBlockSEInfo();
    compress->recordCompress("Total", m_info.originalSize, m_info.compressSize);
    timer1->stop();
}

void EncodeSEWorker::getBlockRead(MemBuf *ptr)
{
    int step = 1;
    char *pbuf = ptr->m_bufptr[0];
    int j = 1;
    int idlen = 0, shlen = 0, seqlen = 0;
    m_blockparamPtr->m_onech = ptr->m_bonech;
    m_blockparamPtr->m_blockidx = ptr->m_blockidx;
    for(int i=0;i<ptr->m_len[0];i++)
    {
        if(pbuf[i] == '\n')
        {
            switch (step)
            {
            case 1:
            {
                step++;
                idlen = i-j;
                m_nameElPtr->m_rawBuf.append(&pbuf[j], idlen);
                m_namelenElPtr->m_LenArry.push_back(idlen);
                j = i+1;
            }
                break;
            case 2:
            {
                step++;
                seqlen = i-j;
                m_seqElPtr->m_rawBuf.append(&pbuf[j], seqlen);
                m_seqLenElPtr->m_LenArry.push_back(seqlen);
                j = i+1;
            }
                break;
            case 3:
            {
                step++;
                // shlen = i-j;
                // m_nameShElPtr->m_rawBuf.append(&pbuf[j], shlen);
                j = i+1;
            }
                break;
            case 4:
            {
                step = 1;
                m_qualElPtr->m_rawBuf.append(&pbuf[j], seqlen);
                j = i+2;
            }
                break;
            default:
                break;
            }
        }
    }
    m_blockparamPtr->m_readcount = m_seqLenElPtr->m_LenArry.size();
    m_memBufPoolPtr->addEmptybuf(ptr); 
}



void EncodeSEWorker::addBlockSEInfo()
{
    std::lock_guard<std::mutex> tlock(m_mtx_BlockInfo);
    if(m_paramPtr->m_vecBlockInfoSE.empty())
    {
        m_info.compressoffset = ARCHEADLEN;
    }
    else
    {
        BlockInfoSE &last = m_paramPtr->m_vecBlockInfoSE.back();
        m_info.compressoffset = last.compressSize+last.compressoffset;
    }
    m_paramPtr->m_vecBlockInfoSE.emplace_back(m_info);
    writeData(m_paramPtr->m_outfd[0], m_outbuf.getdata(), m_info.compressoffset, m_info.compressSize);
}






