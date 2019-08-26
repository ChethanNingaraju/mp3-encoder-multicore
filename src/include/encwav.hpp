#ifndef ENCWAV_H
#define ENCWAV_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <fstream>
#include <pthread.h>
#include <windows.h>

#include <lame.h>

#define LINUX_X64
#ifdef LINUX_X64
#include <dirent.h>
#endif

class EncWAV
{
public:
    EncWAV(const std::string dataDir, const uint32_t numCores);
    static void * callMultiThreadEnc(void *arg)
    {
        return ((EncWAV*)arg)->encodeFileMultithread();
    }

    void process();

    uint32_t encodeFile(const std::string fileName);
    void *encodeFileMultithread();
private:
    std::vector<std::string> m_allFilePath;
    std::vector<uint32_t> m_isFileProcessed; //atomic variable
    uint32_t m_fileCount;
    uint32_t m_numThreads;
    pthread_mutex_t m_lock;
};

#endif // ENCWAV_H
