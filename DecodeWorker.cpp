#include "DecodeWorker.h"

DecodeWorker::DecodeWorker(int num, IRef* refptr):
    IWorker(num, refptr)
{
    m_file.open(m_paramPtr->m_strArcFile, std::ios::binary | std::ios::in);
    m_inbuf.mem_malloc(m_paramPtr->m_iBlockSize);
}

DecodeWorker::~DecodeWorker()
{
}

void DecodeWorker::doJob()
{
    auto timer1 = m_profile->getTimer();

    int count = m_paramPtr->m_vecBlockMeta.size();
    for(uint32_t i=0;i<count;i++)
    {
        if(i % m_paramPtr->m_iThreadnum != m_num)
        {
            continue;
        }

        timer1->start("ReadTime");
        elementClear();

        readData(i);
        timer1->stop();

        timer1->start("DecodeTime");
        decodeData();
        timer1->stop();

        timer1->start("RecoverTime");
        uint32_t len = 0;
        if(m_blockparamPtr->m_onech)
        {
            len = recoverDataNoComment();
        }
        else
        {
            len = recoverDataWithComment();
        }
        timer1->stop();

        timer1->start("WriteTime");
        outPutData(len); 
        timer1->stop();
        m_processcount++;
    }

    m_file.close();
}

void DecodeWorker::readData(uint32_t idx)
{
    BlockMetaInfo &info = m_paramPtr->m_vecBlockMeta[idx];
    m_file.seekg(info.compressOffset);
    m_file.read(m_inbuf.getdata(), info.compressSize);

    m_readfileidx = info.fileidx;
    m_blocksize[0] = info.originalSize1;
    m_blocksize[1] = info.originalSize2;
    m_blockoffset[0] = info.originalOffset1;
    m_blockoffset[1] = info.originalOffset2;

}

void DecodeWorker::decodeData()
{
    EncapInfo info;
    char *dataPtr = m_inbuf.getdata();
    Encap::getValue(dataPtr, info);
    if(info.val != BLOCK_ENCAP)
    {
        arc_stdout(level::ERROR, "Wrong block id");
        exit(1);
    }
    dataPtr += info.len;
    Encap::getValue(dataPtr, info);
    dataPtr += info.len;
    uint32_t out_len = info.val;
    uint32_t readlen = 0;

    while (readlen < out_len)
    {
        Encap::getValue(dataPtr, info);
        int id = info.val;
        dataPtr += info.len;
        readlen += info.len;
        Encap::getValue(dataPtr, info);
        dataPtr += info.len;
        readlen += info.len;

        doRealDecode(id, info.val, dataPtr); 

        dataPtr += info.val;
        readlen += info.val;
    }
}

void DecodeWorker::doRealDecode(int id, uint32_t size, char* data)
{
    auto timer1 = m_profile->getTimer();
    switch (id)
    {
    case BLOCK_PARAM:
        m_paramProcessPtr->decompress(size, data);
        break;
    case BLOCK_SEQLEN:
        timer1->start("SeqLenTime");
        m_seqlenProcessPtr->decompress(size, data);
        timer1->stop();
        break;
    case BLOCK_NAMELEN:
        m_nameLenProcessPtr->decompress(size, data);
        break;
    case BLOCK_NAME:
        timer1->start("NameTime");
        m_nameProcessPtr->decompress(size, data);
        timer1->stop();
        break;
    case BLOCK_SEQ:
        timer1->start("SeqTime");
        m_seqProcessPtr->decompress(size, data);
        timer1->stop();
        break;
    case BLOCK_QUAL:
        timer1->start("QualTime");
        m_qualProcessPtr->decompress(size, data);
        timer1->stop();
        break;
    case BLOCK_ORDER:
        break;  
    default:
        break;
    }
}


int DecodeWorker::copyReadDataNoComment(int i, char *strout)
{   
    char *pout = strout;
    *pout++ = '@';
    memcpy(pout, m_nameElPtr->m_bufPtr, m_namelenElPtr->m_LenArry[i]); //name
    pout += m_namelenElPtr->m_LenArry[i];
    m_nameElPtr->m_bufPtr += m_namelenElPtr->m_LenArry[i];
    *pout++ = '\n';

    memcpy(pout, m_seqElPtr->m_bufPtr, m_seqLenElPtr->m_LenArry[i]); //seq
    pout += m_seqLenElPtr->m_LenArry[i];
    m_seqElPtr->m_bufPtr += m_seqLenElPtr->m_LenArry[i];

    memcpy(pout, "\n+\n", 3); //comment
    pout += 3;

    memcpy(pout, m_qualElPtr->m_bufPtr, m_seqLenElPtr->m_LenArry[i]); //qual
    pout += m_seqLenElPtr->m_LenArry[i];
    m_qualElPtr->m_bufPtr += m_seqLenElPtr->m_LenArry[i];
    *pout++ = '\n';

    return pout-strout;
}

int DecodeWorker::copyReadDataWithComment(int i, char *strout)
{
    char *pout = strout;
    *pout++ = '@';
    memcpy(pout, m_nameElPtr->m_bufPtr, m_namelenElPtr->m_LenArry[i]); //name
    pout += m_namelenElPtr->m_LenArry[i];
    *pout++ = '\n';

    memcpy(pout, m_seqElPtr->m_bufPtr, m_seqLenElPtr->m_LenArry[i]); //seq
    pout += m_seqLenElPtr->m_LenArry[i];
    m_seqElPtr->m_bufPtr += m_seqLenElPtr->m_LenArry[i];
    *pout++ = '\n';

    *pout++ = '+';
    memcpy(pout, m_nameElPtr->m_bufPtr, m_namelenElPtr->m_LenArry[i]); //comment
    pout += m_namelenElPtr->m_LenArry[i];
    m_nameElPtr->m_bufPtr += m_namelenElPtr->m_LenArry[i];
    *pout++ = '\n';

    memcpy(pout, m_qualElPtr->m_bufPtr, m_seqLenElPtr->m_LenArry[i]); //qual
    pout += m_seqLenElPtr->m_LenArry[i];
    m_qualElPtr->m_bufPtr += m_seqLenElPtr->m_LenArry[i];
    *pout++ = '\n';

    return pout-strout;
}