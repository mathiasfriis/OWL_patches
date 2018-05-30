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


#ifndef ___TremoloPatch_hpp__
#define __TremoloPatch_hpp__

#include "CircularBuffer.hpp"
#include "lfo.hpp"

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
  }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
    unsigned int delaySamples;
      
    rate     = (getParameterValue(PARAMETER_A)-1) * 30/100;
    depth    = getParameterValue(PARAMETER_B)/100;
    waveshape = getParameterValue(PARAMETER_C); // so we keep a -3dB summation of the delayed signal
    mode = sine;

    lfo.setFrequency(rate);
      
    for (int ch = 0; ch<buffer.getChannels(); ++ch) {
        for (int i = 0 ; i < size; i++) {
            float* buf = buffer.getSamples(ch);
            lfo.updateLFO_value();
            float dry = buf[i]*depth/100;
            float wet = buf[i]*lfo.get_LFO_value()*(100-depth)/100;
            buf[i] = buf[i]*lfo.get_LFO_value();
        }
    }
  }
    
};


#endif /* __FlangerPatch_hpp__ */