#ifndef __LFO__
#define __LFO__

#include <math.h>
//
#define myPI 3.14159265
#define UINT32MAX 4294967296

//Update 1.1 - Woo, sample&hold now works!

enum lfo_mode{sine,square,triangle,sampleHold,walk};

	class LFO
	{
	private:
	    float fs, frequency, phase, LFO_value, LFO_value_target, waveshape, walkTime, walkSpeedPrSample;
		bool waveShapeStatusState = false;
	    lfo_mode mode;
		bool isPhaseUnderWaveshapeLevel();
		int randomNumber = 100;
        int cnt=0;
        unsigned int lfsr113_Bits();
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
        LFO_value=1;
        LFO_value_target=1;
        waveshape=50;
        mode = sine;
        walkTime=0.3; //Seconds to go from minVal(0) to maxVal(1);
    }
    void LFO::setSampleRate(float sampleRate)
    {
        fs=sampleRate;
        walkSpeedPrSample=1/(fs*walkTime);
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
                LFO_value=sin(phase)/2+0.5;
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
                   
                    unsigned int randomInt = lfsr113_Bits();
                    LFO_value = (float)(randomInt/(pow(2,32)));
    				
                    if(LFO_value<0) LFO_value=0;
                    if(LFO_value>1) LFO_value=1;
    				}
                    //update waveShape status state
                    waveShapeStatusState = isPhaseUnderWaveshapeLevel();
			}
            break;
            case walk:
            {
                // Triggers at waveShape-level and at 0 ('ish)
                if (isPhaseUnderWaveshapeLevel() != waveShapeStatusState)
                {
                   
                    unsigned int randomInt = lfsr113_Bits();
                    LFO_value_target = (float)(randomInt/(pow(2,32)));
                    
                    if(LFO_value_target<0) LFO_value_target=0;
                    if(LFO_value_target>1) LFO_value_target=1;
                    }

                    //if current value is lower than target value
                    if(LFO_value<LFO_value_target)
                    {
                        //If difference is lower than stepSize, stabilize.
                        if(abs(LFO_value_target-LFO_value)<walkSpeedPrSample)
                        {
                            LFO_value=LFO_value_target;
                        }
                        else
                        {
                            LFO_value+=walkSpeedPrSample;
                        }
                    }
                    else
                    {
                        //If difference is lower than stepSize, stabilize.
                        if(abs(LFO_value_target-LFO_value)<walkSpeedPrSample)
                        {
                            LFO_value=LFO_value_target;
                        }
                        else
                        {
                            LFO_value-=walkSpeedPrSample;
                        }
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

    //returns a random number between 0:2^32
    unsigned int LFO::lfsr113_Bits()
    {
       static unsigned int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
       unsigned int b;
       b  = ((z1 << 6) ^ z1) >> 13;
       z1 = ((z1 & 4294967294U) << 18) ^ b;
       b  = ((z2 << 2) ^ z2) >> 27; 
       z2 = ((z2 & 4294967288U) << 2) ^ b;
       b  = ((z3 << 13) ^ z3) >> 21;
       z3 = ((z3 & 4294967280U) << 7) ^ b;
       b  = ((z4 << 3) ^ z4) >> 12;
       z4 = ((z4 & 4294967168U) << 13) ^ b;
       return (z1 ^ z2 ^ z3 ^ z4);
    }

#endif