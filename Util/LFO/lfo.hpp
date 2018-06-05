#ifndef __LFO__
#define __LFO__

#include <math.h>

#define myPI 3.14159265

enum lfo_mode{sine,square,triangle,sampleHold};

	class LFO
	{
	private:
	    float fs, frequency, phase, LFO_value, waveshape;
		bool waveShapeStatusState = false;
	    lfo_mode mode;
		bool isPhaseUnderWaveshapeLevel();
		int randomNumber = 0;
	public:
	    void initLFO();
	    void setSampleRate(float sampleRate);
	    void setLFO_mode(lfo_mode newMode);
	    void setFrequency(float freq);
        void setWaveshape(float Waveshape);
	    void updateLFO_value();
		void feedRandomNumber(int r);
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

	void LFO::feedRandomNumber(int r)
	{
		randomNumber = r % 100;
	}

    void LFO::updateLFO_value()
    {
        phase=(phase+2*myPI/fs*frequency);
		phase = std::fmod(phase,(float)(2 * myPI));
        switch(mode)
        {
            case sine: 
                LFO_value=sin(phase)/2+1;
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
			{
				float dydt_UP = 1 / (2 * myPI / 100 * waveshape); //slope of ramp when going up.
				float dydt_DOWN = 1 / (2 * myPI / 100 * (100 - waveshape)); //slope of ramp when going down.
				if (isPhaseUnderWaveshapeLevel())
				{
					LFO_value = phase*dydt_UP;
				}
				else
				{
					LFO_value = 1 - (phase - waveshape * 2 * myPI / 100)*dydt_DOWN;
				}
				break;	
			}
				

			case sampleHold:
			{
				// Triggers at waveShape-level and at 0 ('ish)
				if (isPhaseUnderWaveshapeLevel() != waveShapeStatusState)
				{
					//LFO_value = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					LFO_value = (randomNumber) / 100;
				}
                //update waveShape status state
                waveShapeStatusState = isPhaseUnderWaveshapeLevel();
				break;
			}
				
        }
    }

    float LFO::get_LFO_value()
    {
        return LFO_value;
    }

	bool LFO::isPhaseUnderWaveshapeLevel()
	{
		if (phase * 100 / (2 * myPI)<waveshape)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

#endif