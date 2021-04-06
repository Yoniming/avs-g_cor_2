/*
 * @Author: zhaozijian 
 * @Date: 2019-09-04 14:35:46 
 * @Last Modified by:   zhaozijian 
 * @Last Modified time: 2019-09-04 14:35:46 
 */
#ifndef AVS_API_IRef_H
#define AVS_API_IRef_H

#include "util.h"
#include "Param.h"

struct AlignInfo
{
    bool m_bRev;    //是否是反向比对
    uint64_t m_alignpos; //比对上的参考序列的pos
    Memory<uint32_t> m_cigarL; //变异碱基的位置
    Memory<uint32_t> m_cigarV; //变异碱基的变化
};

class IRef
{
public:
    IRef()
    {
        m_paramPtr = Param::GetInstance();
    }
    virtual ~IRef(){};
    virtual bool buildRefIndex() = 0;
    virtual bool loadRefIndex() = 0;
    virtual bool freeRefIndex() = 0;

    virtual int doSEAlign(AlignInfo &info) = 0;
    virtual int doPEAlign(AlignInfo &info1, AlignInfo &info2) = 0;
    virtual int query(uint64_t alignPos, int seqlen, uint8_t *refseq) = 0;
public:
    uint32_t m_offsetBit = 30; //偏移的位数
    uint64_t m_offsetSize = 0; //偏移的数值
    uint64_t m_referencelen = 0; //参考序列的碱基个数
protected:
    Param *m_paramPtr = nullptr;
};

#endif //AVS_API_IRef_H