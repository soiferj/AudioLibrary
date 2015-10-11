/*
  ==============================================================================

    ResonantFilter.h
    Created: 11 Oct 2015 12:03:24am
    Author:  Jon Soifer
    Notes: Based off of Paul Kellett's resonant filter algorithm
           http://www.musicdsp.org/showone.php?id=29,
           modified to be used as low-pass, high-pass, and band-pass filter

  ==============================================================================
*/

#ifndef RESONANTFILTER_H_INCLUDED
#define RESONANTFILTER_H_INCLUDED

class ResonantFilter
{
public:
    enum FilterMode
    {
        LowPass = 0,
        HighPass,
        BandPass
    };
    
    ResonantFilter();
    
    void SetCutoff(double newCutoff);
    void SetResonance(double newResonance);
    void SetFilterMode(FilterMode newFilterMode);
    
    double FilterSample(double sample);
    void FilterSampleBuffer(double* sampleBuffer, int size); // note: modifies sample buffer
    
private:
    double cutoff;
    double resonance;
    FilterMode filterMode;
    
    double feedback;
    
    double buf0;
    double buf1;
    
    void CalculateFeedback();
};



#endif  // RESONANTFILTER_H_INCLUDED
