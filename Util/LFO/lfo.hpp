#ifndef __LFO__
#define __LFO__

#include <math.h>
//
#define myPI 3.14159265
#define UINT32MAX 4294967296

static unsigned int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
enum lfo_mode{sine,square,triangle,sampleHold};

	class LFO
	{
	private:
	    float fs, frequency, phase, LFO_value, waveshape;
		bool waveShapeStatusState = false;
	    lfo_mode mode;
		bool isPhaseUnderWaveshapeLevel();
		int randomNumber = 100;
        int cnt=0;
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
        cnt++;
        if(cnt==48000)
        {
            cnt=0;
        }
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
                    unsigned int b;
                    b  = ((z1 << 6) ^ z1) >> 13;
                    z1 = ((z1 & 4294967294U) << 18) ^ b;
                    b  = ((z2 << 2) ^ z2) >> 27; 
                    z2 = ((z2 & 4294967288U) << 2) ^ b;
                    b  = ((z3 << 13) ^ z3) >> 21;
                    z3 = ((z3 & 4294967280U) << 7) ^ b;
                    b  = ((z4 << 3) ^ z4) >> 12;
                    z4 = ((z4 & 4294967168U) << 13) ^ b;
                    
                    unsigned int randomInt=(z1 ^ z2 ^ z3 ^ z4); //get random number between -2^31 and 2^31.

                    //Scale random number to 0:1;
                    randomNumber=(float)randomInt/UINT32MAX;
    				
                    LFO_value = randomNumber;
    				}
                    //update waveShape status state
                    waveShapeStatusState = isPhaseUnderWaveshapeLevel();
			}
            break;

				
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