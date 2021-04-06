#ifndef AVS_API_MEMORY_H
#define AVS_API_MEMORY_H

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include<cstring>
template<typename T>
class Memory
{
public:
    Memory()
    {
    }

    ~Memory()
    {
        if(m_data)
        {
            free(m_data);
            m_data = nullptr;
            m_size = 0;
            m_capacity = 0;
        }
    }

    T* mem_malloc(int size)
    {
        m_capacity = size;
        m_data = (T*)malloc(m_capacity*sizeof(T));
        assert(m_data);
        return m_data;
    }

    T* mem_calloc(int size)
    {
        m_capacity = size;
        m_data = (T*)calloc(m_capacity, sizeof(T));
        assert(m_data);
        return m_data;
    }

    T* mem_realloc(int size)
    {
        m_capacity = size;
        m_data = (T*)realloc(m_data, m_capacity*sizeof(T));
        assert(m_data);
        return m_data;
    }

    void clear()
    {
        m_size = 0;
    }

    void push_back(T val)
    {
        if(m_capacity==m_size)
        {   
            m_capacity *= 2; 
            m_data = (T*)realloc(m_data, m_capacity*sizeof(T));
        }
        m_data[m_size++] = val;
    }

    void append(const T *ptr, int num)
    {
        if(m_capacity-m_size<=num)
        {   
            m_capacity += 2*num; 
            m_data = (T*)realloc(m_data, m_capacity*sizeof(T));
        }
        memcpy(m_data+m_size, ptr, num*sizeof(T));
        m_size += num;
    }

    void copy(T *ptr, int num)
    {
        if(m_capacity<num)
        {   
            m_capacity += 2*num; 
            m_data = (T*)realloc(m_data, m_capacity*sizeof(T));
        }
        memcpy(m_data, ptr, num*sizeof(T));
        m_size = num;
    }

    T &operator[](int idx)
    {
        //assert(idx<m_capacity);
        return m_data[idx];
    }

    void swap(Memory<T> &arry)
    {
        T *tmp = this->m_data;
        this->m_data = arry.getdata();
        arry.setdata(tmp);
        int sizet = this->m_size;
        this->m_size = arry.size();
        arry.setsize(sizet);
        int capacityt = this->m_capacity;
        this->m_capacity = arry.capacity();
        arry.setcapacity(capacityt);
    }


    T *getdata(){return m_data;}
    T *getCurPtr()
    {
        return &m_data[m_size];
    }
    void updatesize(int size)
    {
        m_size += size;
    }
    int size(){return m_size;}
    int capacity(){return m_capacity;}
    void setdata(T *ptr)
    {
        m_data = ptr;
    }
    void setsize(int size)
    {
        m_size = size;
    }
    void setcapacity(int capacity)
    {
        m_capacity = capacity;
    }
private:
    T *m_data = nullptr;
    int m_size = 0; //实际个数,不是字节数
    int m_capacity = 0; //最大个数
};

#endif