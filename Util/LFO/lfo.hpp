#include <math.h>

#ifndef __LFO__
#define __LFO__

#define PI 3.14159265

enum lfo_mode{sine,square,triangle,sampleHold};

	class LFO
	{
	private:
	    float fs, frequency, phase, LFO_value, waveshape;
	    lfo_mode mode;
		bool isPhaseUnderWaveshapeLevel();
	public:
	    void initLFO();
	    void setSampleRate(float sampleRate);
	    void setLFO_mode(lfo_mode newMode);
	    void setFrequency(float freq);
        void setWaveshape(float Waveshape);
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

    void LFO::setWaveshape(float ws)
    {
        waveshape=ws;
    }

    void LFO::updateLFO_value()
    {
        phase=(phase+2*PI/fs*frequency);
		phase = std::fmod(phase,(float)(2 * PI));
        switch(mode)
        {
            case sine: 
                LFO_value=sin(phase);
                break;
            case square:
                if(isPhaseUnderWaveshapeLevel())
                {
                     LFO_value=1;
                }
                else
                {
                    LFO_value=0;
                }
                break;

			case triangle:
				float dydt_UP = 1 / (2 * PI / 100 * waveshape); //slope of ramp when going up.
				float dydt_DOWN = 1 / (2 * PI / 100 * (100-waveshape)); //slope of ramp when going down.
				if (isPhaseUnderWaveshapeLevel())
				{
					LFO_value = phase*dydt_UP;
				}
				else
				{
					LFO_value = 1 - (phase - waveshape * 2 * PI/100)*dydt_DOWN;
				}
        }
    }

    float LFO::get_LFO_value()
    {
        return LFO_value;
    }

	bool LFO::isPhaseUnderWaveshapeLevel()
	{
		if (phase * 100 / (2 * PI)<waveshape)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

#endif