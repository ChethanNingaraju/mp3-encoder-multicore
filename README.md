# LAME based multicore cross-platform audio encoder

## 1. General Usage

This program compiles audio file in .wav format to mp3 files using LAME encoder.

### 1.1 Future Work
[] Currenly, the program assumes .wav files with stereo, 16-bit depth and 44100 Hz. This can be exteneded to other types of input files.

## 2.Linux Instructions

### 2.1 Build the file
'''
cd build_linux
cmake ..
make
'''
### 2.3 Run
'''
./encoder-multi-core ../data
'''

## Windows Instruction
'''
cmake -G "MinGW Makefiles" ..
'''
