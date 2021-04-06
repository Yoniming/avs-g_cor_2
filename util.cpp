#include "util.h"
#include <stdarg.h>

uint64_t getFileSize(const char *path) 
{
    uint64_t flength = 0;
    struct stat statbuf;
    int ret = stat(path, &statbuf);
    if (ret == -1) {
        return 0;
    }

    flength = statbuf.st_size;
    return flength;
}

bool isFileExist(const char *path)
{
    struct stat statbuf;
    int ret = stat(path, &statbuf);
    if (ret == -1) {
        return false;
    }
    return true;
}

bool isValidSrc(const char *path)
{
    string strRef(path);
    string strSuffix = strRef.substr(strRef.rfind('.')+1);
    if(strSuffix == "fq" || strSuffix == "fastq" || strSuffix == "gz")
    {
        return true;
    }
    return false;
}

bool isValidRef(const char *path)
{
    string strRef(path);
    string strSuffix = strRef.substr(strRef.rfind('.')+1);
    if(strSuffix == "fa" || strSuffix == "fasta")
    {
        return true;
    }
    return false;
}

void strSplit(const std::string &str, const std::string &delimiters, std::vector<string> &vectocken)
{
    vectocken.clear();
    string::size_type lastpos = str.find_first_not_of(delimiters, 0);
    string::size_type pos = str.find_first_of(delimiters, lastpos);
    while (string::npos != lastpos || string::npos != pos)
    {
        vectocken.emplace_back(str.substr(lastpos, pos-lastpos));
        lastpos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastpos);
    }
}

void splitPath(const char *path, string &dir, string &name)
{
    dir.clear();
    name.clear();
    string strpath(path);
    int pos = strpath.rfind('/');
    if(pos!=string::npos)
    {
        dir.append(strpath.substr(0, pos+1));
        name.append(strpath.substr(pos+1, strpath.length() -1));
    }
    else
    {
        name.append(path);
    }
}

void IntTo4Ch(int num, char *pbuf)
{
    *pbuf++ = (num >> 0) & 0xff;
    *pbuf++ = (num >> 8) & 0xff;
    *pbuf++ = (num >> 16) & 0xff;
    *pbuf++ = (num >> 24) & 0xff;
}

void IntTo2Ch(int num, char *pbuf)
{
    *pbuf++ = (num >> 0) & 0xff;
    *pbuf++ = (num >> 8) & 0xff;
}

void getFileName(string &str)
{
    int pos1 = str.rfind('/');
    int pos2 = str.find('.', pos1);
    int pos3 = str.find('.', pos2+1);
    pos1++;
    if(pos3 == string::npos)
    {
        str = str.substr(pos1);
    }
    else
    {
        str = str.substr(pos1, pos3-pos1);
    }
}

bool writeData(int fd, char *pbuf, uint64_t offset, uint64_t len)
{
    int ret = lseek(fd, offset, SEEK_SET);
    if(ret == -1)
    {
        printf("lseek data error\n");
        exit(1);
    }
    ret = write(fd, pbuf, len);
    if(ret == -1)
    {
        printf("write data error\n");
        exit(1);
    }
    return true;
}