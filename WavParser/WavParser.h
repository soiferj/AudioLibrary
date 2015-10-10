//
//  WavParser.h
//
//  Allows for reading in a .wav file,
//  and easy access to data
//
//  Created by Jon Soifer on 1/30/15.
//
//

#pragma once

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <string>
#include <exception>

using namespace std;

class WavParser
{
public:
    static const uint16_t NUM_CHARS = 4;
    static const int NUM_BITS_IN_BYTE = 8;
    
    WavParser();
    
    ~WavParser();
    
    void ReadFromFile(string &fileToRead);
    
    string GetFileName();
    string GetAbridgedFileName();
    
    inline uint16_t GetNumChannels() { return numChannels; };
    inline double* GetBuffer1() { return buffer1; };
    inline double* GetBuffer2() { return buffer2; };
    inline long GetNumSamplesPerChannel() { return numSamplesPerChannel; };
    
private:
    string fileName;
    
    char chunkID[NUM_CHARS]; // should say "RIFF"
    uint32_t chunkSize;
    char format[NUM_CHARS]; // should say "WAVE"
    char subChunkID[NUM_CHARS]; // should say "fmt "
    uint32_t subChunkSize;
    uint16_t audioFormat; // PCM = 1
    uint16_t numChannels; // 1 = Mono, 2 = Stereo
    uint32_t sampleRate; // usually 44100
    uint32_t byteRate; // = sampleRate * numChannels * bitsPerSample/8
    uint16_t blockAlign; // = numChannels * bitsPerSample/8
    uint16_t bitsPerSample; // 8 bits = 8; 16 bits = 16; etc
    char subChunk2ID[NUM_CHARS]; // should say "data"
    uint32_t subChunk2Size; // = numSamples * numChannels * bitsPerSample/8 (number of bytes in the data)
    int8_t* rawData; // raw data
    
    /* in case we see "list" chunks and not jump straight into a data chunk */
    char junkChunkID[NUM_CHARS];
    uint32_t junkChunkSize;
    int8_t* junkData;
    
    int bytesPerSample;
    
    /* final audio information */
    long numSamplesPerChannel;
    double* buffer1; // filled if mono, left channel if stereo
    double* buffer2; // NULL if mono, right channel if stereo
    
    void getAudioInformation();
    double bytesToDouble(int8_t* bytes, int numBytes);
    
    void zeroOutBuffer(double* buffer, long size);
};

class WavParserException : public exception
{
public:
    WavParserException(string ss)
        : errorMessage(ss)
    {}

    const char* what() const throw()
    {
       return errorMessage.c_str();
    } 

private:
    string errorMessage;
}
