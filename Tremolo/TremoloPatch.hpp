////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 
 
 LICENSE:
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

/* created by the OWL team 2013 */

////////////////////////////////////////////////////////////////////////////////////////////////////
//
#include "CircularBuffer.hpp"
#include "lfo.hpp"

#ifndef ___TremoloPatch_hpp__
#define __TremoloPatch_hpp__

#define FLANGER_BUFFER_SIZE 1024

class TremoloPatch : public Patch {
private:
    float rate, depth, waveshape;
    lfo_mode mode;
    LFO lfo;

    
public:
  TremoloPatch(){
    AudioBuffer* buffer = createMemoryBuffer(1, FLANGER_BUFFER_SIZE);
    registerParameter(PARAMETER_A, "Rate");
    registerParameter(PARAMETER_B, "Depth");
    registerParameter(PARAMETER_C, "Waveshape");
    registerParameter(PARAMETER_D, "Waveform");
    lfo.initLFO();
    float fs = getSampleRate();
    lfo.setSampleRate(fs);

	srand(1);
  }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
    unsigned int delaySamples;
      
    rate     = (getParameterValue(PARAMETER_A)*30);
    depth    = getParameterValue(PARAMETER_B);
    waveshape = getParameterValue(PARAMETER_C)*100; // so we keep a -3dB summation of the delayed signal
    float waveFormFloat = getParameterValue(PARAMETER_D)*100;
    if(waveFormFloat<25)
    {
        lfo.setLFO_mode(sine);
    }
    else if(waveFormFloat>25 && waveFormFloat < 50)
    {
        lfo.setLFO_mode(square);
    }
	else if (waveFormFloat>50 && waveFormFloat < 75)
	{
		lfo.setLFO_mode(triangle);
	}
	else
	{
		lfo.feedRandomNumber(getElapsedCycles);
		lfo.setLFO_mode(sampleHold);
	}

    lfo.setWaveshape(waveshape);
    lfo.setFrequency(rate);
      
    for (int ch = 0; ch<buffer.getChannels(); ++ch) {
        for (int i = 0 ; i < size; i++) {
            float* buf = buffer.getSamples(ch);
            lfo.updateLFO_value();
            float dry = buf[i]*(1-depth);
            float wet = buf[i]*lfo.get_LFO_value()*(depth);
            buf[i] = dry+wet;
        }
    }
  }
    
};


#endif /* __FlangerPatch_hpp__ */