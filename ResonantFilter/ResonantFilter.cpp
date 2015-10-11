/*
  ==============================================================================

    ResonantFilter.cpp
    Created: 11 Oct 2015 12:03:24am
    Author:  Jon Soifer

  ==============================================================================
*/

#include "ResonantFilter.h"

ResonantFilter::ResonantFilter()
: cutoff(0.99), resonance(0.0), filterMode(LowPass), feedback(0.0), buf0(0.0), buf1(0.0)
{}

void ResonantFilter::SetCutoff(double newCutoff)
{
    cutoff = newCutoff;
    CalculateFeedback();
}

void ResonantFilter::SetResonance(double newResonance)
{
    resonance = newResonance;
    CalculateFeedback();
}

void ResonantFilter::SetFilterMode(ResonantFilter::FilterMode newFilterMode)
{
    filterMode = newFilterMode;
}

double ResonantFilter::FilterSample(double sample)
{
    buf0 = buf0 + cutoff * (sample - buf0 + feedback * (buf0 - buf1) );
    buf1 = buf1 + cutoff * (buf0 - buf1);
    
    switch (filterMode)
    {
        case LowPass:
            return buf1;
            break;
            
        case HighPass:
            return (sample - buf1);
            break;
            
        case BandPass:
            return (buf0 - buf1);
            break;
            
        default:
            return 0.0;
    }
}

void ResonantFilter::FilterSampleBuffer(double* sampleBuffer, int size)
{
    for (int i = 0; i < size; i++, sampleBuffer++)
    {
        *sampleBuffer = FilterSample(*sampleBuffer);
    }
}

void ResonantFilter::CalculateFeedback()
{
    feedback = resonance + (resonance / (1.0 - cutoff) );
}
