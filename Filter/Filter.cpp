/*
  ==============================================================================

    Filter.cpp
    Created: 16 Oct 2015 2:38:26pm
    Author:  Jon Soifer

  ==============================================================================
*/

#include "Filter.h"
#include <algorithm>
#include <cstring>

using std::max;
using std::min;

Filter::Filter()
:   delayLine(nullptr), outputs(nullptr), length(0), index(0), aCoefficients(nullptr), bCoefficients(nullptr), aLength(0), bLength(0)
{
}

Filter::Filter(double* aCoeffs, int aLen, double* bCoeffs, int bLen)
:   delayLine(nullptr), outputs(nullptr), length(0), index(0), aCoefficients(nullptr), bCoefficients(nullptr), aLength(0), bLength(0)
{
    SetCoefficients(aCoeffs, aLen, bCoeffs, bLen);
}

Filter::~Filter()
{
    if (aCoefficients != nullptr)
        delete[] aCoefficients;
    
    if (bCoefficients != nullptr)
        delete[] bCoefficients;
    
    if (delayLine != nullptr)
        delete[] delayLine;
}

void Filter::SetCoefficients(double* aCoeffs, int aLen, double* bCoeffs, int bLen)
{
    /* reset coefficients */
    if (aCoefficients != nullptr)
        delete[] aCoefficients;
    
    aCoefficients = new double[aLen];
    for (int i = 0; i < aLen; i++)
    {
        aCoefficients[i] = aCoeffs[i];
    }
    
    aLength = aLen;
    
    if (bCoeffs != nullptr) // only set b coefficients if they are passed in
    {
        if (bCoefficients != nullptr)
            delete[] bCoefficients;
        
        bCoefficients = new double[bLen];
        for (int i = 0; i < bLen; i++)
        {
            bCoefficients[i] = bCoeffs[i];
        }
        
        bLength = bLen;
    }
    
    /* reset delayLine and outputs buffers if necessary */
    int maxLength = max(aLen, bLen);
    if (length != maxLength)
    {
        ResetBufferLength(delayLine, maxLength);
        ResetBufferLength(outputs, maxLength);
        length = maxLength;
        index = (index >= maxLength) ? 0 : index;
    }
}

double Filter::FilterSample(double sample)
{
    if (aCoefficients == nullptr) // if coefficients haven't been set yet
        return 0.0;
    
    double output = 0.0;
    
    for (int i = 0; i < length; i++)
    {
        if (i < aLength)
        {
            output += bCoefficients[i]*delayLine[index];
        }
        
        if (i < bLength)
        {
            output -= aCoefficients[i]*outputs[index];
        }
        
        IncrementIndex();
    }
    
    double denom = (aCoefficients[0] != 0.0) ? aCoefficients[0] : 1.0;
    output /= denom;
    
    /* store new sample/output and "shift" buffer */
    DecrementIndex(); // index now is at oldest sample
    delayLine[index] = sample;
    outputs[index] = output; // this is now the newest sample, same as shifting buffer
    
    return output;
}

void Filter::ResetBufferLength(double* buffer, int newLength)
{
    double* tempBuffer = new double[newLength];
    memset(tempBuffer, 0.0, newLength);
    memcpy(tempBuffer, buffer, min(length, newLength)); // accounts for it new buffer is smaller or larger than original
    
    if (buffer != nullptr)
        delete[] buffer;
    
    buffer = tempBuffer;
}

void Filter::IncrementIndex()
{
    index++;
    if (index >= length)
        index = 0;
}

void Filter::DecrementIndex()
{
    index--;
    if (index < 0)
        index = length - 1;
}
