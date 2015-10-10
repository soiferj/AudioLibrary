//
//  WavParser.cpp
//
//  Created by Jon Soifer on 1/30/15.
//
//

#include "WavParser.h"
#include "math.h"

WavParser::WavParser()
:   fileName(""),
    rawData(NULL),
    junkData(NULL),
    buffer1(NULL),
    buffer2(NULL)
{
}


WavParser::~WavParser()
{
    if (rawData != NULL)
    {
        delete[] rawData;
    }
    
    if (junkData != NULL)
    {
        delete[] junkData;
    }
    
    if (buffer1 != NULL)
    {
        delete[] buffer1;
    }
    
    if (buffer2 != NULL)
    {
        delete[] buffer2;
    }
}


/* reads the .wav file and stores data into corresponding members */
void WavParser::ReadFromFile(string &fileToRead)
{
    fileName = fileToRead;
    
    ifstream infile;
    const char* cFileName = fileToRead.c_str();
    
    /* attempt to open the file */
    infile.open(cFileName, ifstream::binary);
    if ( !infile.good() )
    {
        throw FidgetException("Error: Unable to open file...");
    }
    
    /* read chunkID */
    infile.read(chunkID, WavParser::NUM_CHARS);
    if ( memcmp(chunkID, "RIFF", 4) )
    {
        throw FidgetException("Error: Not a RIFF file...");
    }
    
    /* read chunk size */
    infile.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));
    
    /* read format */
    infile.read(format, WavParser::NUM_CHARS);
    if ( memcmp(format, "WAVE", 4) )
    {
        throw FidgetException("Error: Not a WAV file...");
    }
    
    /* read "fmt " */
    infile.read(subChunkID, WavParser::NUM_CHARS);
    while ( memcmp(subChunkID, "fmt ", 4) ) // if we don't see fmt , store all of the rest of the stuff in header until we actually see it
    {
        infile.read(reinterpret_cast<char*>(&junkChunkSize), sizeof(junkChunkSize));
        junkData = new int8_t[junkChunkSize];
        infile.read(reinterpret_cast<char*>(junkData), junkChunkSize);
        infile.read(subChunkID, WavParser::NUM_CHARS);
    }
    
    
    /* read size of fmt chunk */
    infile.read(reinterpret_cast<char*>(&subChunkSize), sizeof(subChunkSize));
    
    /* read audio format, make sure it's PCM */
    infile.read(reinterpret_cast<char*>(&audioFormat), sizeof(audioFormat));
    if (audioFormat != 1)
    {
        throw FidgetException("Error: unable to read compressed audio file...");
    }
    
    /* read number of channels */
    infile.read(reinterpret_cast<char*>(&numChannels), sizeof(numChannels));
    if ( (numChannels != 1) && (numChannels != 2) )
    {
        throw FidgetException("Error: not a mono or stereo file, unable to construct buffers...");
    }
    
    /* read sample rate */
    infile.read(reinterpret_cast<char*>(&sampleRate), sizeof(sampleRate));
    
    /* read byte rate */
    infile.read(reinterpret_cast<char*>(&byteRate), sizeof(byteRate));
    
    /* read block align */
    infile.read(reinterpret_cast<char*>(&blockAlign), sizeof(blockAlign));
    
    /* read bits per sample */
    infile.read(reinterpret_cast<char*>(&bitsPerSample), sizeof(bitsPerSample));
    
    /* read data id */
    infile.read(subChunk2ID, WavParser::NUM_CHARS);
    while ( memcmp(subChunk2ID, "data", 4) ) // if we don't see data, store all of the rest of the stuff in list until we actually see it
    {
        infile.read(reinterpret_cast<char*>(&junkChunkSize), sizeof(junkChunkSize));
        junkData = new int8_t[junkChunkSize];
        infile.read(reinterpret_cast<char*>(junkData), junkChunkSize);
        infile.read(subChunk2ID, WavParser::NUM_CHARS);
    }
    
    /* read size of data */
    infile.read(reinterpret_cast<char*>(&subChunk2Size), sizeof(subChunk2Size));
    
    /* allocate and read data */
    rawData = new int8_t[subChunk2Size];
    infile.read(reinterpret_cast<char*>(rawData), subChunk2Size);
    
    getAudioInformation();
}


/* determines number of samples, and stores doubles into correct buffers */
void WavParser::getAudioInformation()
{
    /* calculate total number samples and how many bytes to a sample */
    bytesPerSample = blockAlign / numChannels;
    numSamplesPerChannel = (subChunk2Size / numChannels) / bytesPerSample;
    
    /* allocate and store data to new buffers */
    if (numChannels == 1)
    {
        buffer1 = new double[numSamplesPerChannel];
        zeroOutBuffer(buffer1, numSamplesPerChannel);
        
        /* take each sample and store it in corresponding buffers */
        int8_t* bytes = new int8_t[bytesPerSample];
        int rawDataIndex = 0;
        for (int i = 0; i < numSamplesPerChannel; i++)
        {
            for (int j = 0; j < bytesPerSample; j++)
            {
                bytes[j] = rawData[rawDataIndex++];
            }
            buffer1[i] = bytesToDouble(bytes, bytesPerSample);
        }
        delete[] bytes;
    }
    else
    {
        buffer1 = new double[numSamplesPerChannel];
        buffer2 = new double[numSamplesPerChannel];
        zeroOutBuffer(buffer1, numSamplesPerChannel);
        zeroOutBuffer(buffer2, numSamplesPerChannel);
        
        /* take each sample and store it in corresponding buffers */
        int8_t* bytes1 = new int8_t[bytesPerSample];
        int8_t* bytes2 = new int8_t[bytesPerSample];
        int rawDataIndex = 0;
        for (int i = 0; i < numSamplesPerChannel; i++)
        {
            for (int j = 0; j < bytesPerSample; j++)
            {
                bytes1[j] = rawData[rawDataIndex++];
            }
            for (int j = 0; j < bytesPerSample; j++)
            {
                bytes2[j] = rawData[rawDataIndex++];
            }
            buffer1[i] = bytesToDouble(bytes1, bytesPerSample);
            buffer2[i] = bytesToDouble(bytes2, bytesPerSample);
        }
        delete[] bytes1;
        delete[] bytes2;
    }
}


/* takes in array of bytes, converts them to integer, then normalizes it by bitsPerSample */
double WavParser::bytesToDouble(int8_t* bytes, int numBytes)
{
    double normalizedData = 0;
    
    for (int i = 0; i < numBytes; i++)
    {
        if (i == numBytes - 1) // only the last byte should be signed
        {
            normalizedData += (bytes[i] << i*NUM_BITS_IN_BYTE);
        }
        else // everything else should be unsigned
        {
            normalizedData += ((uint8_t)bytes[i] << i*NUM_BITS_IN_BYTE);
        }
    }
    
    normalizedData /= pow(2, bitsPerSample - 1); // because each sample is a signed 16-bit double
    
    return normalizedData;
}


/* takes in array of doubles, and sets them all to zero */
void WavParser::zeroOutBuffer(double* buffer, long size)
{
    for (int i = 0; i < size; i++)
    {
        buffer[i] = 0;
    }
}


string WavParser::GetFileName()
{
    return fileName;
}


/* finds "abridged" file name - i.e. file name without absolute path in front of it
   NOTE: this is kind of a convoluted way of getting to the name (working backwards then
         reversing that string), but there are only regex libraries in c++11 */
string WavParser::GetAbridgedFileName()
{
    string abridgedFileNameReverse = "";
    for (long i = fileName.length() - 1; i >= 0; i--)
    {
        if (fileName[i] == '/')
        {
            break;
        }
        abridgedFileNameReverse.push_back(fileName[i]);
    }
    
    string abridgedFileName = "";
    for (long i = abridgedFileNameReverse.length() - 1; i >= 0; i--)
    {
        abridgedFileName.push_back(abridgedFileNameReverse[i]);
    }
    
    return abridgedFileName;
}
