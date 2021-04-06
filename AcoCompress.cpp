#include "clr.h"
#include "simple_model.h"
#include "AcoCompress.h"
#include <cstring>
#include <algorithm>

#define MAX_SYMBOL 45
#define NUM_SYMBOL 38
#define XX_SYMBOL (MAX_SYMBOL-NUM_SYMBOL)
using namespace std;
// This function is used to compress the aligned quality score file 
int AcoCompress::aco_compress(char *dest,uint64_t *destLen,const uint8_t *source, const uint8_t *seq_source,const uint32_t *len_arr,uint64_t read_num)
{
    //===============================1.1 Parameter definition & Initialization=======//
    int rows = read_num;
    int cols = len_arr[0];
    int sourceLen = rows*cols;
    char *quality_block = new char[sourceLen];
    memcpy(quality_block,source,sizeof(unsigned char)*sourceLen);
    char *base_block = new char[sourceLen];
    memcpy(base_block,seq_source,sizeof(unsigned char)*sourceLen);
    int model_num = NUM_SYMBOL*NUM_SYMBOL*NUM_SYMBOL * 16;
    char table[256];
    memset(table, 0, 256);
	table['A'] = 0;
	table['T'] = 1;
	table['C'] = 2;
	table['G'] = 3;
	table['N'] = 4;
    char min_1 = '~';
    for(int tmp = 0;tmp<sourceLen;tmp++)
    {
        if(quality_block[tmp]<min_1)
            min_1 = quality_block[tmp];
    }
    //===============================1.2 Encoder definition=========================//
	int T = XX_SYMBOL - 1;
	SIMPLE_MODEL<NUM_SYMBOL+1> *model_qual;
	model_qual = new SIMPLE_MODEL<NUM_SYMBOL+1>[model_num + 1];
    RangeCoder rc;
    rc.output(dest);
    rc.StartEncode();

    //=====================================2 Process==============================//
    int i = 0, offset, model_idx;
    for (int j = 0; j < cols; j++)
    {
        for (int k = 0; k < rows; k++)
        {
            if (j % 2 == 0)
            {
                i = k;
            }
            else
            {
                i = rows - k - 1;
            }
            offset = i*cols + j;
            quality_block[offset] = quality_block[offset] - min_1;
            char cur_base = base_block[offset];
            int J0 = table[cur_base];
            if (J0 == 4)
            {
                model_idx = 0;
            }
            else
            {
                if (j == 0)
                {
                    model_idx = J0 + 1;
                }
                else if (j == 1)
                {
                    char pre_base_1 = base_block[offset - 1];
                    int J1 = table[pre_base_1];
                    int G1 = J0 * 5 + J1;
                    int Q1 = quality_block[offset - 1];
                    model_idx = Q1 * 20 + G1 + 5;
                }
                else if (j == 2)
                {
                    char pre_base_1 = base_block[offset - 1];
                    int J1 = table[pre_base_1];
                    int G1 = J0 * 5 + J1;
                    int Q1 = quality_block[offset - 1];
                    int Q2 = quality_block[offset - 2];
                    model_idx = (Q1*NUM_SYMBOL + Q2) * 20 + G1 + 5 + NUM_SYMBOL * 20;
                }
                else
                {
                    char pre_base_1 = base_block[offset - 1];
                    char pre_base_2 = base_block[offset - 2];
                    int J1 = table[pre_base_1];
                    int J2 = table[pre_base_2];
                    int G1 = J0 * 5 + J1;
                    int G2 = J0 * 4 * 4 + J1 * 4 + J2;
                    int Q1 = quality_block[offset - 1];
                    int Q2 = quality_block[offset - 2];
                    int Q3 = quality_block[offset - 3];
                    int Q4 = 0;
                    if (j == 3)
                    {
                        Q4 = Q3;
                    }
                    else
                    {
                        int Q4 = quality_block[offset - 4];
                    }
                    int A = Q1;
                    int B = max(Q2, Q3);
                    int C = 0;
                    if (Q2 == Q3)
                    {
                        C = 1;
                    }
                    if (Q3 == Q4)
                    {
                        C = 2;
                    }
                    model_idx = A * NUM_SYMBOL * 3  * 20 + B * 3  * 20 + C * 20  + G1 + 5 + NUM_SYMBOL * 20 + NUM_SYMBOL*NUM_SYMBOL * 20;
                }
            }
            if (quality_block[offset]>T)
            {
                model_qual[model_idx].encodeSymbol(&rc, quality_block[offset]-T);
            }
            else
            {
                model_qual[model_idx].encodeSymbol(&rc, 0);
                model_qual[model_num].encodeSymbol(&rc, quality_block[offset]);
            }
        }
    }
    rc.FinishEncode();
	int sz1; 
    sz1 = rc.size_out();
    dest[sz1] = min_1;
    *destLen = sz1+1;
    delete[] model_qual;
    delete[] quality_block;
	delete[] base_block;
    // delete[] out0;
}

// This function is used to compress the unaligned quality score file 
int AcoCompress::aco_compress_unalign(char *dest,uint64_t *destLen,const uint8_t *source, const uint8_t *seq_source,const uint32_t *len_arr,uint64_t read_num)
{
    //===============================1.1 Parameter definition & Initialization=======//
    int rows = read_num;
    int sourceLen = *destLen;
    char *quality_block = new char[sourceLen];
    memcpy(quality_block,source,sizeof(unsigned char)*sourceLen);
    char *base_block = new char[sourceLen];
    memcpy(base_block,seq_source,sizeof(unsigned char)*sourceLen);
    int model_num = NUM_SYMBOL*NUM_SYMBOL*NUM_SYMBOL * 16;
    char table[256];
    memset(table, 0, 256);
	table['A'] = 0;
	table['T'] = 1;
	table['C'] = 2;
	table['G'] = 3;
	table['N'] = 4;
    char min_1 = '~';
    for(int tmp = 0;tmp<sourceLen;tmp++)
    {
        if(quality_block[tmp]<min_1)
            min_1 = quality_block[tmp];
    }
    // dest[0]=min_1;
    // memcpy(&dest[0],&min_1,sizeof(unsigned char));

    //===============================1.2 Encoder definition=========================//
	int T = XX_SYMBOL - 1;
	SIMPLE_MODEL<NUM_SYMBOL+1> *model_qual;
	model_qual = new SIMPLE_MODEL<NUM_SYMBOL+1>[model_num + 1];
    RangeCoder rc;
    rc.output(dest);
    rc.StartEncode();

    //=====================================2 Process==============================//
    int cur=0,offset=0, model_idx;
    for (int i = 0; i< rows; i++)
    {
        cur += len_arr[i];
        for (int j = 0; j < len_arr[i]; j++)
        {
            if(i==0)
                offset = j;
            else
                offset = cur-len_arr[i] + j;

            quality_block[offset] = quality_block[offset] - min_1;
            char cur_base = base_block[offset];
            int J0 = table[cur_base];
            if (J0 == 4)
            {
                model_idx = 0;
            }
            else
            {
                if (j == 0)
                {
                    model_idx = J0 + 1;
                }
                else if (j == 1)
                {
                    char pre_base_1 = base_block[offset - 1];
                    int J1 = table[pre_base_1];
                    int G1 = J0 * 5 + J1;
                    int Q1 = quality_block[offset - 1];
                    model_idx = Q1 * 20 + G1 + 5;
                }
                else if (j == 2)
                {
                    char pre_base_1 = base_block[offset - 1];
                    int J1 = table[pre_base_1];
                    int G1 = J0 * 5 + J1;
                    int Q1 = quality_block[offset - 1];
                    int Q2 = quality_block[offset - 2];
                    model_idx = (Q1*NUM_SYMBOL + Q2) * 20 + G1 + 5 + NUM_SYMBOL * 20;
                }
                else
                {
                    char pre_base_1 = base_block[offset - 1];
                    char pre_base_2 = base_block[offset - 2];
                    int J1 = table[pre_base_1];
                    int J2 = table[pre_base_2];
                    int G1 = J0 * 5 + J1;
                    int G2 = J0 * 4 * 4 + J1 * 4 + J2;
                    int Q1 = quality_block[offset - 1];
                    int Q2 = quality_block[offset - 2];
                    int Q3 = quality_block[offset - 3];
                    int Q4 = 0;
                    if (j == 3)
                    {
                        Q4 = Q3;
                    }
                    else
                    {
                        int Q4 = quality_block[offset - 4];
                    }
                    int A = Q1;
                    int B = max(Q2, Q3);
                    int C = 0;
                    if (Q2 == Q3)
                    {
                        C = 1;
                    }
                    if (Q3 == Q4)
                    {
                        C = 2;
                    }
                    model_idx = A * NUM_SYMBOL * 3  * 20 + B * 3  * 20 + C * 20 + G1 + 5 + NUM_SYMBOL * 20 + NUM_SYMBOL*NUM_SYMBOL * 20;
                }
            }
            if (quality_block[offset]>T)
            {
                model_qual[model_idx].encodeSymbol(&rc, quality_block[offset]-T);
            }
            else
            {
                model_qual[model_idx].encodeSymbol(&rc, 0);
                model_qual[model_num].encodeSymbol(&rc, quality_block[offset]);
            }
        }
    }
    rc.FinishEncode();
	int sz1; 
    sz1 = rc.size_out();
    dest[sz1] = min_1;
    *destLen = sz1+1;
    // printf("sz1:%d\n",sz1);
    delete[] model_qual;
    delete[] quality_block;
	delete[] base_block;
    // delete[] out0; 
}

// This function is used to decompress the aligned quality score file 
int AcoCompress::aco_decompress(uint8_t *dest,uint64_t *destLen,const uint8_t *source, const uint8_t *seq_source,const uint32_t *len_arr,int inlen,uint64_t read_num)
{
    //===============================1.1 Parameter definition & Initialization=======//
    int cols = len_arr[0];
    int rows = read_num;
    int sourceLen = rows*cols;
    char *decoded_matrix = new char[sourceLen];
    char *in_buf1 = new char[inlen];
    memcpy(in_buf1,source,sizeof(unsigned char)*inlen);
	char *base_block = new char[sourceLen];
    memcpy(base_block,seq_source,sizeof(unsigned char)*sourceLen);
    int model_num = NUM_SYMBOL*NUM_SYMBOL*NUM_SYMBOL * 16;
    char table[256];
    memset(table, 0, 256);
	table['A'] = 0;
	table['T'] = 1;
	table['C'] = 2;
	table['G'] = 3;
	table['N'] = 4;
    char min_1 = source[inlen-1];
    //===============================1.2 Encoder definition=========================//
    int T = XX_SYMBOL - 1;
    SIMPLE_MODEL<NUM_SYMBOL + 1> *model_qual;
	model_qual = new SIMPLE_MODEL<NUM_SYMBOL + 1>[model_num + 1];
    RangeCoder rc;
    rc.input(in_buf1);
    rc.StartDecode();

    //=====================================2 Process==============================//
    int i = 0, offset, model_idx;
    for (int j = 0; j < cols; j++)
    {
        for (int k = 0; k < rows; k++)
        {
            if (j % 2 == 0)
            {
                i = k;
            }
            else
            {
                i = rows - k - 1;
            }
            offset = i*cols + j;
            char cur_base = base_block[offset];
            int J0 = table[cur_base];
            if (J0 == 4)
            {
                model_idx = 0;
            }
            else
            {
                if (j == 0)
                {
                    model_idx = J0 + 1;
                }
                else if (j == 1)
                {
                    char pre_base_1 = base_block[offset - 1];
                    int J1 = table[pre_base_1];
                    int G1 = J0 * 5 + J1;
                    int Q1 = decoded_matrix[offset - 1];
                    model_idx = Q1 * 20 + G1 + 5;
                }
                else if (j == 2)
                {
                    char pre_base_1 = base_block[offset - 1];
                    int J1 = table[pre_base_1];
                    int G1 = J0 * 5 + J1;
                    int Q1 = decoded_matrix[offset - 1];
                    int Q2 = decoded_matrix[offset - 2];
                    model_idx = (Q1*NUM_SYMBOL + Q2) * 20 + G1 + 5 + NUM_SYMBOL * 20;
                }
                else
                {
                    char pre_base_1 = base_block[offset - 1];
                    char pre_base_2 = base_block[offset - 2];
                    int J1 = table[pre_base_1];
                    int J2 = table[pre_base_2];
                    int G1 = J0 * 5 + J1;
                    int G2 = J0 * 4 * 4 + J1 * 4 + J2;
                    int Q1 = decoded_matrix[offset - 1];
                    int Q2 = decoded_matrix[offset - 2];
                    int Q3 = decoded_matrix[offset - 3];
                    int Q4 = 0;
                    if (j == 3)
                    {
                        Q4 = Q3;
                    }
                    else
                    {
                        int Q4 = decoded_matrix[offset - 4];
                    }
                    int A = Q1;
                    int B = max(Q2, Q3);
                    int C = 0;
                    if (Q2 == Q3)
                    {
                        C = 1;
                    }
                    if (Q3 == Q4)
                    {
                        C = 2;
                    }
                    model_idx = A * NUM_SYMBOL * 3  * 20 + B * 3  * 20 + C * 20  + G1 + 5 + NUM_SYMBOL * 20 + NUM_SYMBOL*NUM_SYMBOL * 20;
                }
            }
            decoded_matrix[offset] = model_qual[model_idx].decodeSymbol(&rc);
            if (int(decoded_matrix[offset]) == 0)
            {
                decoded_matrix[offset] = model_qual[model_num].decodeSymbol(&rc);
            }
            else
            {
                decoded_matrix[offset] = decoded_matrix[offset] + T;
            }
            // decoded_matrix[offset] = decoded_matrix[offset] + '!';
        }
    }
    for(int tmp = 0;tmp<sourceLen;tmp++)
    {
        decoded_matrix[tmp] = decoded_matrix[tmp] + min_1;
    }
    
    rc.FinishDecode();
    memcpy(dest,decoded_matrix,sizeof(unsigned char)*sourceLen);
    *destLen = sourceLen;
    delete[] model_qual;
    delete[] in_buf1;
    delete[] base_block;
    delete[] decoded_matrix;
}  

// This function is used to decompress the unaligned quality score file 
int AcoCompress::aco_decompress_unalign(uint8_t *dest,uint64_t *destLen,const uint8_t *source, const uint8_t *seq_source,const uint32_t *len_arr,int inlen,uint64_t read_num,uint64_t size_file)
{
    //===============================1.1 Parameter definition & Initialization=======//
    int rows = read_num;
    int sourceLen = size_file;
    char *decoded_matrix = new char[sourceLen];
    char *in_buf1 = new char[inlen];
    memcpy(in_buf1,source,sizeof(unsigned char)*inlen);
	char *base_block = new char[sourceLen];
    memcpy(base_block,seq_source,sizeof(unsigned char)*sourceLen);
    int model_num = NUM_SYMBOL*NUM_SYMBOL*NUM_SYMBOL * 16;
    char table[256];
    memset(table, 0, 256);
	table['A'] = 0;
	table['T'] = 1;
	table['C'] = 2;
	table['G'] = 3;
	table['N'] = 4;
    char min_1 = source[inlen-1];
    //===============================1.2 Encoder definition=========================//
    int T = XX_SYMBOL - 1;
    SIMPLE_MODEL<NUM_SYMBOL + 1> *model_qual;
	model_qual = new SIMPLE_MODEL<NUM_SYMBOL + 1>[model_num + 1];
    RangeCoder rc;
    rc.input(in_buf1);
    rc.StartDecode();

    //=====================================2 Process==============================//
    int cur=0,offset=0, model_idx;
    for (int i = 0; i< rows; i++)
    {
        cur += len_arr[i];
        for (int j = 0; j < len_arr[i]; j++)
        {
            if(i==0)
                offset = j;
            else
                offset = cur-len_arr[i] + j;

            char cur_base = base_block[offset];
            int J0 = table[cur_base];
            if (J0 == 4)
            {
                model_idx = 0;
            }
            else
            {
                if (j == 0)
                {
                    model_idx = J0 + 1;
                }
                else if (j == 1)
                {
                    char pre_base_1 = base_block[offset - 1];
                    int J1 = table[pre_base_1];
                    int G1 = J0 * 5 + J1;
                    int Q1 = decoded_matrix[offset - 1];
                    model_idx = Q1 * 20 + G1 + 5;
                }
                else if (j == 2)
                {
                    char pre_base_1 = base_block[offset - 1];
                    int J1 = table[pre_base_1];
                    int G1 = J0 * 5 + J1;
                    int Q1 = decoded_matrix[offset - 1];
                    int Q2 = decoded_matrix[offset - 2];
                    model_idx = (Q1*NUM_SYMBOL + Q2) * 20 + G1 + 5 + NUM_SYMBOL * 20;
                }
                else
                {
                    char pre_base_1 = base_block[offset - 1];
                    char pre_base_2 = base_block[offset - 2];
                    int J1 = table[pre_base_1];
                    int J2 = table[pre_base_2];
                    int G1 = J0 * 5 + J1;
                    int G2 = J0 * 4 * 4 + J1 * 4 + J2;
                    int Q1 = decoded_matrix[offset - 1];
                    int Q2 = decoded_matrix[offset - 2];
                    int Q3 = decoded_matrix[offset - 3];
                    int Q4 = 0;
                    if (j == 3)
                    {
                        Q4 = Q3;
                    }
                    else
                    {
                        int Q4 = decoded_matrix[offset - 4];
                    }
                    int A = Q1;
                    int B = max(Q2, Q3);
                    int C = 0;
                    if (Q2 == Q3)
                    {
                        C = 1;
                    }
                    if (Q3 == Q4)
                    {
                        C = 2;
                    }
                    model_idx = A * NUM_SYMBOL * 3  * 20 + B * 3  * 20 + C * 20 + G1 + 5 + NUM_SYMBOL * 20 + NUM_SYMBOL*NUM_SYMBOL * 20;
                }
            }
            decoded_matrix[offset] = model_qual[model_idx].decodeSymbol(&rc);
            if (int(decoded_matrix[offset]) == 0)
            {
                decoded_matrix[offset] = model_qual[model_num].decodeSymbol(&rc);
            }
            else
            {
                decoded_matrix[offset] = decoded_matrix[offset] + T;
            }
            // decoded_matrix[offset] = decoded_matrix[offset] + '!';
        }
    }
    for(int tmp = 0;tmp<sourceLen;tmp++)
    {
        decoded_matrix[tmp] = decoded_matrix[tmp] + min_1;
    }
    rc.FinishDecode();
    memcpy(dest,decoded_matrix,sizeof(unsigned char)*sourceLen);
    *destLen = sourceLen;
    delete[] model_qual;
    delete[] in_buf1;
    delete[] base_block;
    delete[] decoded_matrix;
}
