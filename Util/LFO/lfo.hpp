#include <math.h>

#ifndef __LFO__
#define __LFO__

enum lfo_mode{sine,square,triangle,sampleHold};

	class LFO
	{
	private:
	    float fs, frequency, phase, LFO_value;
	    lfo_mode mode;
	public:
	    void initLFO();
	    void setSampleRate(float sampleRate);
	    void setLFO_mode(lfo_mode newMode);
	    void setFrequency(float freq);
	    void updateLFO_value();
	    float get_LFO_value();
	};


	void LFO::initLFO()
    {
        phase=0;
        frequency=1;
        LFO_value=0;
    }
    void LFO::setSampleRate(float sampleRate)
    {
        fs=sampleRate;
    }
    void LFO::setLFO_mode(lfo_mode newMode)
    {
        mode = newMode;
    }
    void LFO::setFrequency(float freq)
    {
        frequency = freq;
    }
    void LFO::updateLFO_value()
    {
        phase=(phase+360/(fs/frequency));
        if(phase>=360) phase-=360;
        switch(mode)
        {
            case sine: LFO_value=sin(phase);
        }
    }

    float LFO::get_LFO_value()
    {
        return LFO_value;
    }


#endif