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
#include <dirent.h>

#include <lame.h>

class EncWAV
{
public:
    EncWAV(const std::string dataDir, const uint32_t numCores);
    static void * callMultiThreadEnc(void *arg)
    {
        return ((EncWAV*)arg)->encodeFileMultithread();
    }

    void process();

private:
    std::vector<std::string> m_allFilePath;
    std::vector<uint32_t> m_isFileProcessed; //atomic variable
    uint32_t m_fileCount;
    uint32_t m_numThreads;
    pthread_mutex_t m_lock;

    uint32_t encodeFile(const std::string fileName);
    void *encodeFileMultithread();
};

#endif // ENCWAV_H
