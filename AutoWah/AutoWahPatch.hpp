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
#ifndef ___AutoWahPatch_hpp__
#define __AutoWahPatch_hpp__

#include "CircularBuffer.hpp"
#include "lfo.hpp"
#include "SVF.hpp"

#define FLANGER_BUFFER_SIZE 1024
#define CUTOFF_MIN 0
#define CUTOFF_SCALER 8000
#define Q_SCALER 30
#define RATE_SCALER 100
#define DEPTH_SCALER 8000

class AutoWahPatch : public Patch {
private:
    float rate, depth, Q, fc;
    lfo_mode mode;
    LFO lfo;
    StateVariableFilter filter;

    
public:
  AutoWahPatch(){
    AudioBuffer* buffer = createMemoryBuffer(1, FLANGER_BUFFER_SIZE);
    registerParameter(PARAMETER_A, "Rate");
    registerParameter(PARAMETER_B, "Depth");
    registerParameter(PARAMETER_C, "Q");
    registerParameter(PARAMETER_D, "Cutoff");
    lfo.initLFO();
    float fs = getSampleRate();
    lfo.setSampleRate(fs);
    filter.initFilter(fs);

    lfo.setWaveshape(sine);
    filter.setFilterType(BAND_PASS);
  }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
    unsigned int delaySamples;
      
    rate     = (getParameterValue(PARAMETER_A)*RATE_SCALER);
    depth    = getParameterValue(PARAMETER_B)*DEPTH_SCALER;
    Q = getParameterValue(PARAMETER_C)/100*Q_SCALER; // so we keep a -3dB summation of the delayed signal
    fc= getParameterValue(PARAMETER_D)*CUTOFF_SCALER/100+CUTOFF_MIN;
    
    filter.setQfactor(10);
    //filter.setCutoff(fc+lfo.get_LFO_value()*depth);
    filter.setCutoff(fc);
    
    lfo.setFrequency(rate);
      
    for (int ch = 0; ch<buffer.getChannels(); ++ch) {
        for (int i = 0 ; i < size; i++) {
            float* buf = buffer.getSamples(ch);
            lfo.updateLFO_value();

            float dry = buf[i]*(1-1);
            float wet = filter.doFiltering(buf[i])*1;
            buf[i] = dry+wet;
        }
    }
  }
    
};


#endif /* __FlangerPatch_hpp__ */