#ifndef SVT_HPP_
#define SVT_HPP_
#include <stdint.h>
#include <math.h>

typedef enum
{
	LOW_PASS = 0,
	BAND_PASS = 1,
	HIGH_PASS = 2
} SVF_FILTER_TYPE;


class StateVariableFilter
{
private:
	float hpf = 0;
	float bpf = 0;
	float lpf = 0;
	float Q = 0;
	float q = 0;
	float f = 0;
	float fc = 0;
	float sampleRate = 0;
	SVF_FILTER_TYPE type = BAND_PASS;
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
}

// SVF pass
void StateVariableFilter::SVFpass(float x)
{
    lpf = bpf * f + lpf;
    hpf = q * x - lpf - q * bpf;
    bpf = hpf * f + bpf;
}
// State variable filter
float StateVariableFilter::doFiltering(float x)
{
    float q = 1 / Q;
    float f = sin((3.141593 * fc));
    if(f >= 0.5) f = 0.5;

    // Run two passes
    SVFpass(x);
    SVFpass(x);

    // Return selected filter outputs
    if (type == LOW_PASS) return lpf;
    if (type == HIGH_PASS) return hpf;
    if (type == BAND_PASS) return bpf;
    return 0;
}


#endif