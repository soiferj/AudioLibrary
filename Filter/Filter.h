/*
  ==============================================================================

    Filter.h
    Created: 16 Oct 2015 2:38:26pm
    Author:  Jon Soifer
    Notes: 1D Filter based on rational transfer function.
           If only 'a' coefficients are given, acts as FIR filter;
           If 'a' and 'b' coefficients are given, acts as IIR filter

  ==============================================================================
*/

#ifndef FILTER_H_INCLUDED
#define FILTER_H_INCLUDED

class Filter
{
public:
    Filter();
    Filter(double* aCoeffs, int aLen, double* bCoeffs = nullptr, int bLen = 0);
    ~Filter();
    
    void SetCoefficients(double* aCoeffs, int aLen, double* bCoeffs = nullptr, int bLen = 0);
    double FilterSample(double sample);
    
private:
    double* delayLine;
    double* outputs;
    int length;
    int index; // note, current index-1 also acts as front of buffer
    
    double* aCoefficients;
    double* bCoefficients;
    int aLength;
    int bLength;
    
    void ResetBufferLength(double* buffer, int newLength);
    void IncrementIndex();
    void DecrementIndex();
};



#endif  // FILTER_H_INCLUDED
