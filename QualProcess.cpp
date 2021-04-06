#include "QualProcess.h"
#include "IWorker.h"
#include "AcoCompress.h"


QualProcess::QualProcess(IWorker *ptr)
{
    m_seqElPtr = ptr->getseqElPtr();
    m_qualElPtr = ptr->getqualElPtr();
    m_seqLenElPtr = ptr->getseqLenElPtr();
    m_profile = ptr->m_profile;
}

QualProcess::~QualProcess()
{
}
AcoCompress aco;
int QualProcess::compress(char *outptr)
{
    auto compress = m_profile->getCompress();
    auto timer1 = m_profile->getTimer();
    timer1->start("QualTime");

    uint64_t destlen = m_qualElPtr->m_rawBuf.size();
    uint64_t read_num = m_seqLenElPtr->m_LenArry.size();

    uint32_t *len_arr = m_seqLenElPtr->m_LenArry.getdata();
    uint8_t *psrc = (uint8_t*)m_qualElPtr->m_rawBuf.getdata();

    // DEBUG->mmm
    uint8_t *psrc_seq = (uint8_t*)m_seqElPtr->m_rawBuf.getdata();

    // int ret = compress2((uint8_t*)outptr, &destlen, psrc, m_qualElPtr->m_rawBuf.size(), Z_DEFAULT_COMPRESSION);

    int cal_row = destlen/double(read_num);
    int real_row = m_seqLenElPtr->m_LenArry[0];
    int ret;
    if(cal_row==real_row)
        ret = aco.aco_compress((char*)outptr,&destlen,psrc, psrc_seq,len_arr,read_num);
    else
        ret = aco.aco_compress_unalign((char*)outptr,&destlen,psrc, psrc_seq,len_arr,read_num);

    int rawsize = m_qualElPtr->m_rawBuf.size()+m_seqLenElPtr->m_LenArry.size();
    compress->recordCompress("qual", rawsize, destlen);
    timer1->stop();
    // if(ret != Z_OK)
    // {
    //     arc_stdout(level::ERROR, "QualProcess compress error");
    //     return 0;
    // }
    return destlen;
}

int QualProcess::decompress(int inlen, char *inptr)
{
    uint64_t destlen = m_qualElPtr->m_rawBuf.size();
    uint64_t read_num = m_seqLenElPtr->m_LenArry.size();
    uint8_t *pdest = (uint8_t*)m_qualElPtr->m_rawBuf.getdata();
    uint8_t *psrc_seq = (uint8_t*)m_seqElPtr->m_rawBuf.getdata();
    uint32_t *len_arr = m_seqLenElPtr->m_LenArry.getdata();
    uint64_t size_file = m_seqElPtr->m_rawBuf.size();

    int cal_row = size_file/double(read_num);
    int real_row = m_seqLenElPtr->m_LenArry[0];
    int ret;
    if(cal_row==real_row)
        ret = aco.aco_decompress(pdest,&destlen,(uint8_t*)inptr, psrc_seq,len_arr,inlen,read_num);
    else
        ret = aco.aco_decompress_unalign(pdest,&destlen,(uint8_t*)inptr, psrc_seq,len_arr,inlen,read_num,size_file);

    // int ret = uncompress(pdest, &destlen, (uint8_t*)inptr, inlen);

    // if(ret != Z_OK)
    // {
    //     arc_stdout(level::ERROR, "QualProcess decompress error");
    // }

    m_qualElPtr->m_rawBuf.updatesize(destlen);
    return destlen;
}
