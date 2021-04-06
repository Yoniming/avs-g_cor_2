#include "EncodePEWorker.h"
#include <algorithm>


EncodePEWorker::EncodePEWorker(int num, IRef* refptr):
    EncodeWorker(num, refptr)
{
}

EncodePEWorker::~EncodePEWorker()
{
}

void EncodePEWorker::doTask(MemBuf *ptr)
{
    auto compress = m_profile->getCompress();
    auto timer1 = m_profile->getTimer();

    memset(&m_info, 0, sizeof(BlockInfoPE));
    m_info.originalSize[0] = ptr->m_len[0];
    m_info.originaloffset[0] = ptr->m_offset[0];
    m_info.originalSize[1] = ptr->m_len[1];
    m_info.originaloffset[1] = ptr->m_offset[1];

    timer1->start("GetDataTime");
    elementClear();

    getBlockRead(ptr);
    timer1->stop();

    timer1->start("TotalCompressTime");
    m_info.compressSize = DoCompressData();
    timer1->stop();

    timer1->start("WriteTime");
    addBlockPEInfo();
    compress->recordCompress("Total", m_info.originalSize[0]+m_info.originalSize[1], m_info.compressSize);
    timer1->stop();
}

void EncodePEWorker::getBlockRead(MemBuf *ptr)
{
    char *pbuf1 = ptr->m_bufptr[0];
    char *pbuf2 = ptr->m_bufptr[1];
    int idlen = 0, shlen = 0, seqlen = 0;
    int k1 = 1, k2 = 1, idx = 0;
    m_blockparamPtr->m_onech = ptr->m_bonech;
    m_blockparamPtr->m_blockidx = ptr->m_blockidx;
    while (idx<ptr->m_penum)
    {
        idlen = ptr->m_vecpos1[idx]- k1;
        m_nameElPtr->m_rawBuf.append(&pbuf1[k1], idlen);
        m_namelenElPtr->m_LenArry.push_back(idlen);
        k1 = ptr->m_vecpos1[idx] + 1;

        seqlen = ptr->m_vecpos1[idx+1]-k1;
        m_seqElPtr->m_rawBuf.append(&pbuf1[k1], seqlen);
        m_seqLenElPtr->m_LenArry.push_back(seqlen);
        k1 = ptr->m_vecpos1[idx+2] + 1;

        //shlen = i-j;
        // m_nameShElPtr->m_rawBuf.append(&pbuf[j], shlen);

        m_qualElPtr->m_rawBuf.append(&pbuf1[k1], seqlen);
        k1 = ptr->m_vecpos1[idx+3] + 2;


        idlen = ptr->m_vecpos2[idx]- k2;
        m_nameElPtr->m_rawBuf.append(&pbuf2[k2], idlen);
        m_namelenElPtr->m_LenArry.push_back(idlen);
        k2 = ptr->m_vecpos2[idx] + 1;

        seqlen = ptr->m_vecpos2[idx+1]-k2;
        m_seqElPtr->m_rawBuf.append(&pbuf2[k2], seqlen);
        m_seqLenElPtr->m_LenArry.push_back(seqlen);
        k2 = ptr->m_vecpos2[idx+2] + 1;
        
        //shlen = i-j;
        // m_nameShElPtr->m_rawBuf.append(&pbuf[j], shlen);

        m_qualElPtr->m_rawBuf.append(&pbuf2[k2], seqlen);
        k2 = ptr->m_vecpos2[idx+3] + 2;
        idx += 4;
    }
    m_blockparamPtr->m_readcount = m_seqLenElPtr->m_LenArry.size();
    m_memBufPoolPtr->addEmptybuf(ptr);
}

void EncodePEWorker::addBlockPEInfo()
{
    std::lock_guard<std::mutex> tlock(m_mtx_BlockInfo);
    if(m_paramPtr->m_vecBlockInfoPE.empty())
    {
        m_info.compressoffset = ARCHEADLEN;
    }
    else
    {
        BlockInfoPE &last = m_paramPtr->m_vecBlockInfoPE.back();
        m_info.compressoffset = last.compressSize+last.compressoffset;
    }
    m_paramPtr->m_vecBlockInfoPE.emplace_back(m_info);
    writeData(m_paramPtr->m_outfd[0], m_outbuf.getdata(), m_info.compressoffset, m_info.compressSize);
}