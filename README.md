# LAME based multicore cross-platform audio encoder

## 1. General Usage

This program encodes audio file in .wav format to mp3 files using LAME encoder. 

```
./encoder-multi-core <directory with .wav files>
```

All the .wav/.WAV files in input directory is encoded and creates .mp3 file with same name in the same path. The number of cores on the platform is auto-detected and min(number of cores, number of files to encode) number of threads are created to speed-up the task.

The code has been texted for compilation and functionality on Ubuntu 16.04 and Windows 10 (using mingw64). 

### 1.1 Future Work
- [ ] Currenly, the program assumes .wav files with stereo, 16-bit depth and 44100 Hz. This can be exteneded to other types of input files.
- [ ] The current multi-threading works on parallizing tasks by encoding multiple files simultaneously on multi-cores. This works well for large number of small files to be encoded. However, parallizing a single encoding task can help speed-up when large size audio files but few number of files. 
- [ ] For windows run fix the need the need to place lame.dll in executable folder

## 2.Linux Instructions

### 2.1 Build the file
```
cd build_linux
cmake ..
make
```
### 2.3 Run
```
./encoder-multi-core ../data
```
## 2.Windows Instructions

### 2.3.1 Windows Instruction
```
cd build_windows
cmake -G "MinGW Makefiles" ..
mingw32-make.exe
```

### 2.3.1 Windows Run
```
.\encoder-multi-core.exe ..\data
```
