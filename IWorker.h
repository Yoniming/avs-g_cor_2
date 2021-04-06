#ifndef AVS_API_IWORKER_H
#define AVS_API_IWORKER_H

#include "Param.h"
#include "IRef.h"
#include "BlockParamElement.h"
#include "NameLenElement.h"
#include "NameElement.h"
#include "SeqElement.h"
#include "SeqLenElement.h"
#include "QualEmement.h"
#include "NameShadowElement.h"
#include "BlockParamProcess.h"
#include "NameProcess.h"
#include "NameLenProcess.h"
#include "SeqLenProcess.h"
#include "SeqProcess.h"
#include "QualProcess.h"

enum BLOCK
{
    BLOCK_ENCAP = 1,
    BLOCK_PARAM,
    BLOCK_SEQLEN,
    BLOCK_NAMELEN,
    BLOCK_NAME,
    BLOCK_SEQ,
    BLOCK_QUAL,
    BLOCK_ORDER
};

class IWorker
{
public:
    IWorker(int num, IRef* refptr):m_num(num),m_RefPtr(refptr)
    {
        m_paramPtr = Param::GetInstance();

        char buf[16] = {0};
        sprintf(buf, "worker%d", num);
        m_profile = Profile::getInstance().getModule(buf);

        initElement();
        initPorcesser();
    }

    void initElement()
    {
        m_blockparamPtr = new BlockParamElement();
        m_namelenElPtr = new NameLenElement();
        m_nameElPtr = new NameElement();
        m_seqElPtr = new SeqElement();
        m_seqLenElPtr = new SeqLenElement();
        m_qualElPtr = new QualEmement();
    }

    void initPorcesser()
    {
        m_paramProcessPtr = new BlockParamProcess(this);
        m_nameProcessPtr = new NameProcess(this);
        m_nameLenProcessPtr = new NameLenProcess(this);
        m_seqlenProcessPtr = new SeqLenProcess(this);
        m_seqProcessPtr = new SeqProcess(this);
        m_qualProcessPtr = new QualProcess(this);
    }

    virtual ~IWorker()
    {
        RELEASEPTR(m_namelenElPtr);
        RELEASEPTR(m_nameElPtr);
        RELEASEPTR(m_seqElPtr);
        RELEASEPTR(m_seqLenElPtr);
        RELEASEPTR(m_qualElPtr);
        RELEASEPTR(m_nameLenProcessPtr);
        RELEASEPTR(m_nameProcessPtr);
        RELEASEPTR(m_seqlenProcessPtr);
        RELEASEPTR(m_seqProcessPtr);
        RELEASEPTR(m_qualProcessPtr);
    }
    virtual void doJob() = 0;

    void elementClear()
    {
        m_blockparamPtr->clear();
        m_namelenElPtr->clear();
        m_nameElPtr->clear();
        m_seqElPtr->clear();
        m_seqLenElPtr->clear();
        m_qualElPtr->clear();
    }
    IRef *getRefPtr(){return m_RefPtr;}
    BlockParamElement *getblockparamPtr(){return m_blockparamPtr;}
    NameLenElement *getnamelenElPtr(){return m_namelenElPtr;}
    NameElement *getnameElPtr(){return m_nameElPtr;}
    SeqElement *getseqElPtr(){return m_seqElPtr;}
    SeqLenElement *getseqLenElPtr(){return m_seqLenElPtr;}
    QualEmement *getqualElPtr(){return m_qualElPtr;}
    int getProcessCount(){return m_processcount;} //只是用来显示进度，不要求精确，可以不考虑线程竞态
public:
    std::shared_ptr<Module> m_profile;
protected:
    int m_num = 0; //序号
    int m_processcount = 0; //处理过的数据块个数
    Param *m_paramPtr = nullptr;
    IRef *m_RefPtr = nullptr;

    BlockParamElement *m_blockparamPtr = nullptr;
    NameLenElement *m_namelenElPtr = nullptr;
    NameElement *m_nameElPtr = nullptr;
    SeqElement *m_seqElPtr = nullptr;
    SeqLenElement *m_seqLenElPtr = nullptr;
    QualEmement *m_qualElPtr = nullptr;

    BlockParamProcess *m_paramProcessPtr = nullptr;
    NameProcess *m_nameProcessPtr = nullptr;
    NameLenProcess *m_nameLenProcessPtr = nullptr;
    SeqLenProcess *m_seqlenProcessPtr = nullptr;
    SeqProcess *m_seqProcessPtr = nullptr;
    QualProcess *m_qualProcessPtr = nullptr;
    
    static std::mutex m_mtx_BlockInfo;
};
#endif