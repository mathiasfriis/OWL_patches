#ifndef SVT_HPP_
#define SVT_HPP_
#include <stdint.h>
#include <math.h>

typedef enum
{
	LOW_PASS = 0,
	BAND_PASS = 1,
	HIGH_PASS = 2,
	NOTCH = 3
} SVF_FILTER_TYPE;


class StateVariableFilter
{
private:
	float hpf;
	float bpf;
	float lpf;
	float notch;
	float Q;
	float q;
	float f;
	float fc;
	float sampleRate;
	SVF_FILTER_TYPE type;
    void SVFpass(float);

public:
	void setFilterType(SVF_FILTER_TYPE filter_type);
	void setCutoff(float frequency);
	void setQfactor(float value);
	void initFilter(float fs);
	float doFiltering(float x);
};

//Initialize filter
void StateVariableFilter::initFilter(float fs)
{
	sampleRate=fs;
	hpf=0;
	lpf=0;
	bpf=0;
	fc=0;
	q=0;
	Q=0;
	f=0;
}

// Set filter type
void StateVariableFilter::setFilterType(SVF_FILTER_TYPE filter_type)
{
	type = filter_type;
}

// Set Q-factor
void StateVariableFilter::setQfactor(float value)
{
	Q = value + 1;
}

// Set cut off frequency
void StateVariableFilter::setCutoff(float freq)
{
	fc = freq;
	
	if(fc<0)
	{
		fc=0;
	}
	if(fc>(sampleRate/2*0.9))
	{
		fc=sampleRate/2*0.9;
	}
	
}

// SVF pass
void StateVariableFilter::SVFpass(float x)
{
    lpf = bpf * f + lpf;
    hpf = q * x - lpf - q * bpf;
    bpf = bpf + hpf * f;
}
// State variable filter
float StateVariableFilter::doFiltering(float x)
{
    q = 1 / Q;
    //f = sin((3.141593 * fc/2));
    //f=2*sin(3.141593*fc/sampleRate);
    f=2*(3.141593*fc/sampleRate);
    // Run two passes
    SVFpass(x);
    SVFpass(x);

    // Return selected filter outputs
    if (type == LOW_PASS) return lpf;
    if (type == HIGH_PASS) return hpf;
    if (type == BAND_PASS) return bpf;
    if (type == NOTCH) return x-bpf;
    return 0;
}


#endif