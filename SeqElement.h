#ifndef AVS_API_SEQELEMENT_H
#define AVS_API_SEQELEMENT_H

#include "IElement.h"

class SeqElement:public IElement
{
public:
    SeqElement(){init();}
    virtual ~SeqElement(){}
    virtual void init()
    {
        m_rawBuf.mem_malloc(m_paramPtr->m_iBlockSize/2);
        m_bufPtr = m_rawBuf.getdata();
    }
    virtual void clear()
    {
        m_rawBuf.clear();
        m_bufPtr = m_rawBuf.getdata();
    }

    char *m_bufPtr = nullptr;
    Memory<char> m_rawBuf;
};




#endif