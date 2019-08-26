#include "encwav.hpp"


EncWAV::EncWAV(const std::string dataDir, const uint32_t numCores)
    : m_fileCount(0)
    , m_numThreads(1)
{
    std::cout << "Encoding the WAV files in " << dataDir << std::endl;
    m_numThreads = numCores;
    //parse the path to find all files to encode
    DIR *dataDirStrm = opendir(dataDir.c_str());
    struct dirent *dp;
    while((dp = readdir(dataDirStrm)) != NULL)
    {
        //check for extension
        std::string fileName = std::string(dp->d_name);
        if(fileName.size() > 4 &&
                (fileName.compare((fileName.size() - 4),4,".WAV") == 0 || fileName.compare((fileName.size() - 4),4,".wav") == 0))
        {
            //push full path names of selected files
            m_allFilePath.push_back(dataDir + "\\" + fileName);
            m_isFileProcessed.push_back(0);
            std::cout << m_allFilePath[m_fileCount] << std::endl;
            m_fileCount++;
        }
    }

    closedir(dataDirStrm);

    //create mutex lock for multi-threading
    if (pthread_mutex_init(&m_lock, NULL) != 0)
    {
        printf("\n mutex init has failed\n");
    }
}


#define PCM_IN_SIZE 16384
    //worst case estimate coming from API document: 1.25*num_samples + 7200
#define MP3_BUFFER_SIZE ((5 * 16384 + 4)/4 + 7200)


uint32_t EncWAV::encodeFile(const std::string fileName)
{

    std::string mp3FileName = fileName.substr(0,fileName.find_last_of('.'))+".mp3";

    std::ifstream inFile(fileName.c_str() , std::ios::binary);
    std::ofstream mp3File(mp3FileName.c_str(), std::ios::binary);

    if(!inFile.is_open() || !mp3File.is_open())
    {
        std::cout << "Error: Unable to open Input/Output file " << fileName.c_str() << std::endl;
        return 1;
    }

    lame_global_flags *gfp;
    gfp = lame_init();
    lame_set_quality(gfp,2);

    int32_t ret_code = lame_init_params(gfp);

    if(ret_code < 0)
    {
        std::cerr << "Error: Lame init failed" << std::endl;
    }


    //Read input data into PCM buffer
    short int pcmBuffer[PCM_IN_SIZE * 2];

    unsigned char mp3OutBuffer[MP3_BUFFER_SIZE];


    while(1)
    {
        //read interleaved data to use interleaved encoder API to speed up
        inFile.read(reinterpret_cast<char*>(pcmBuffer),sizeof(pcmBuffer));
        int32_t numSamplesRead = inFile.gcount() >> 2; //2 bytes per sample interleaved = 4 bytes per sample
        uint32_t writeSize = 0;
        if(numSamplesRead > 0)
        {
            writeSize = lame_encode_buffer_interleaved(gfp,pcmBuffer,numSamplesRead,mp3OutBuffer,MP3_BUFFER_SIZE);
        }
        else //flush situation
        {
            writeSize = lame_encode_flush(gfp,mp3OutBuffer,MP3_BUFFER_SIZE);
            break;
        }
        mp3File.write(reinterpret_cast<char*>(mp3OutBuffer),writeSize);
    }

    inFile.close();
    mp3File.close();

    return 0;
}

void *EncWAV::encodeFileMultithread()
{
    while(1) //threads take up task in a loop until all files are encoded
    {
        //get the first in the file list that had not yet been processed
        int32_t curFileProcessId = -1;
        pthread_mutex_lock(&m_lock);
        for(uint32_t i = 0; i < m_fileCount ;i++)
        {
            if(m_isFileProcessed[i] == 0)
            {
                curFileProcessId = i;
                //mark the encoding job as taken
                m_isFileProcessed[i] = 1;
                break;
            }
        }

        pthread_mutex_unlock(&m_lock);

        std::cout << "curFileProcessId " << curFileProcessId << std::endl;
        if(curFileProcessId >= 0)
        {
            //complete processing this file
            encodeFile(m_allFilePath[curFileProcessId]);
            std::cout << "Processed file number = " << curFileProcessId << std::endl;
        }
        else
        {
            std::cout << "No files left: closing thread " <<  std::endl;
            break;
        }

    }
}

void EncWAV::process()
{

    uint32_t numActualthread = std::min(m_fileCount, m_numThreads);

    std::vector<pthread_t> allThread(numActualthread);

    //Continously create new threads so that number of active threads is equal to number of cores
    for(uint32_t i = 0; i < numActualthread; i++)
    {
        uint32_t ret = pthread_create(&allThread[i],  NULL,EncWAV::callMultiThreadEnc,(void *)this);
        if(ret)
        {
            std::cerr << "Unable to create thread " << ret << std::endl;
        }
    }

    for(uint32_t i = 0; i < numActualthread; i++)
    {
        int32_t retVal = pthread_join(allThread[i], NULL);
		if (retVal)
		{
			std::cout << " ERROR: ptherad_join ret value =  " << retVal <<  std::endl;
		}

    }
    std::cout << "All threads done" << std::endl;

}


/*
uint32_t EncWAV::encodeFileMultithread(const std::string fileName, const uint32_t numThreads)
{

   std::string mp3FileName = fileName.substr(0,fileName.find_last_of('.'))+".mp3";

   std::ifstream inFile(fileName.c_str() , std::ios::binary | std::ios::ate);
   std::ofstream mp3File(mp3FileName.c_str(), std::ios::binary);

   if(!inFile.is_open() || !mp3File.is_open())
   {
       std::cerr << "Error: Unable to open Input/Output file " << fileName.c_str() << std::endl;
       return 1;
   }
   uint32_t inFileSize = inFile.tellg();
   inFile.seekg(0, std::ios::beg);
   std::cout << "In file siz = " << inFileSize <<std::endl;
   unsigned char *inputBuffer = static_cast<unsigned char*>(malloc(inFileSize));
   unsigned char *outputBuffer = static_cast<unsigned char*>(malloc(inFileSize));

   //Divide the data in multiple of PCM buffer size
   uint32_t numActualthread = (inFileSize + PCM_IN_SIZE) / (PCM_IN_SIZE * 2); //round off to nearest int
   //clip the number of actual thread to max cores
   numActualthread = std::min(numActualthread,m_numThreads);

   //read the input file
   inFile.read(reinterpret_cast<char*>(inputBuffer), inFileSize);

   //create vec of threads with corresponding offset to work on
   std::vector<pthread_t> allThread(numActualthread);
   pthread_attr_t attr;

   //init thread attributes
   pthread_attr_init(&attr);
   for(uint32_t i; i < numActualthread; i++)
   {
       uint32_t ret = pthread_create(&allThread[i], &attr,wait,(void *) i);
       if(ret)
       {
           std::cerr << "Unable to create thread " << ret << std::endl;
       }
   }
   lame_global_flags *gfp;
   gfp = lame_init();
   lame_set_quality(gfp,2);

   int32_t ret_code = lame_init_params(gfp);

   if(ret_code < 0)
   {
       std::cerr << "Error: Lame init failed" << std::endl;
   }


   //Read input data into PCM buffer
   short int pcmBuffer[PCM_IN_SIZE * 2];

   unsigned char mp3OutBuffer[MP3_BUFFER_SIZE];


   while(1)
   {
       //read interleaved data to use interleaved encoder API to speed up
       inFile.read(reinterpret_cast<char*>(pcmBuffer),sizeof(pcmBuffer));
       int32_t numSamplesRead = inFile.gcount() >> 2; //2 bytes per sample interleaved = 4 bytes per sample
       uint32_t writeSize = 0;
       if(numSamplesRead > 0)
       {
           writeSize = lame_encode_buffer_interleaved(gfp,pcmBuffer,numSamplesRead,mp3OutBuffer,MP3_BUFFER_SIZE);
       }
       else //flush situation
       {
           writeSize = lame_encode_flush(gfp,mp3OutBuffer,MP3_BUFFER_SIZE);
           break;
       }
       mp3File.write(reinterpret_cast<char*>(mp3OutBuffer),writeSize);
   }

   inFile.close();
   mp3File.close();

   return 0;
}*/
