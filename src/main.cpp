#include <iostream>
#include "encwav.hpp"
#include <unistd.h>
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64)
#include <sysinfoapi.h>
#endif
using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "USAGE: encoder.exe <path with .wav audio files>" << std::endl;
		return 0;
	}

    //Get the number of cores
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64)
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int numCores = sysinfo.dwNumberOfProcessors;
#else
    uint32_t numCores = sysconf(_SC_NPROCESSORS_ONLN);
#endif
    std::cout << "Number of cores = " << numCores << std::endl;
    EncWAV encoderWAV(argv[1], numCores);
    //encode all the files
    encoderWAV.process();
    return 0;
}

