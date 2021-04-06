#ifndef AVS_API_UTIL_H
#define AVS_API_UTIL_H

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <cmath>
#include <string>
#include <inttypes.h>
#include "Memory.h"


using namespace std;

/********************************宏定义**********************/
#define DECODE_INT(a) ((a)[0] + ((a)[1]<<8) + ((a)[2]<<16) + ((a)[3]<<24))
#define RELEASEARRYPTR(p) {if(p) {delete[] p;p=nullptr;}}
#define RELEASEPTR(p) {if(p) {delete p; p=nullptr;}}

/********************************常量**********************/
const int MAJOR_VERS = 1;
const int MINOR_VERS = 7;
const int STAGE_VERS = 0;

const int MAX_THREAD_NUM = 100;
const int PATH_LEN = 1024;
const int ARCHEADLEN = 16;
/********************************枚举**************************/

/********************************struct**********************/
typedef struct _tagBlockMetaInfo
{
    uint32_t fileidx;       //多文件压缩时的文件序号
    uint32_t compressSize;  //压缩后大小
    uint32_t originalSize1;  //压缩前大小
    uint32_t originalSize2;  //压缩前大小
    uint64_t compressOffset;  //压缩文件的偏移量
    uint64_t originalOffset1;  //原文件1的偏移量
    uint64_t originalOffset2;  //原文件2的偏移量
}BlockMetaInfo;


typedef struct _tagBlockInfoSE
{
    uint32_t fileidx;       //多文件压缩时的文件序号
    uint32_t compressSize;  //压缩后大小
    uint32_t originalSize;  //压缩前大小
    uint64_t compressoffset;//压缩文件偏移量
    uint64_t originaloffset;//压缩前文件偏移量
}BlockInfoSE;

typedef struct _tagBlockInfoPE
{
    uint32_t compressSize;  //压缩后大小
    uint32_t originalSize[2];  //压缩前大小
    uint64_t compressoffset;//压缩文件偏移量
    uint64_t originaloffset[2];//压缩前文件偏移量
}BlockInfoPE;

typedef struct _tagBlockParamInfo
{
    uint8_t onech; //read第三行是否是单字符
    uint32_t writeidx; //写入时的序号
    uint32_t blockidx; //block序号
    uint32_t readcount; //block的read条数
}BlockParamInfo;

/********************************Function**********************/
uint64_t getFileSize(const char *path);
bool isFileExist(const char *path);
bool isValidSrc(const char *path);
bool isValidRef(const char *path);
void strSplit(const std::string &str, const std::string &delimiters, std::vector<string> &vectocken);
void splitPath(const char *path, string &dir, string &name);
void IntTo4Ch(int num, char *pbuf);
void IntTo2Ch(int num, char *pbuf);
void getFileName(string &str);
bool writeData(int fd, char *pbuf, uint64_t offset, uint64_t len);
#endif //AVS_API_UTIL_H