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
#define CUTOFF_SCALER 2000
#define Q_SCALER 10
#define RATE_SCALER 10
#define DEPTH_SCALER 8000

class AutoWahPatch : public Patch {
private:
    float rate, depth, Q, fc, fc_offset;
    lfo_mode mode;
    LFO lfo;
    StateVariableFilter filter;
    bool buttonState;
    SVF_FILTER_TYPE FilterType=BAND_PASS;

    
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
    //filter.initFilter(44100);
    fc=0;

    lfo.setLFO_mode(triangle);
    lfo.setWaveshape(50);
    filter.setFilterType(LOW_PASS);
  }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
    unsigned int delaySamples;
      
    rate     = (getParameterValue(PARAMETER_A)*RATE_SCALER);
    depth    = getParameterValue(PARAMETER_B)*DEPTH_SCALER;
    Q = getParameterValue(PARAMETER_C)*Q_SCALER; // so we keep a -3dB summation of the delayed signal
    fc_offset= getParameterValue(PARAMETER_D)*CUTOFF_SCALER+CUTOFF_MIN;
    
    filter.setQfactor(Q);
    
    lfo.setFrequency(rate);

    if(buttonState!=isButtonPressed(PUSHBUTTON))
    {
        buttonState=isButtonPressed(PUSHBUTTON);
        if(buttonState==false)
        {
            switch(FilterType)
            {
                case BAND_PASS:
                    FilterType=LOW_PASS;
                    break;
                case LOW_PASS:
                    FilterType=HIGH_PASS;
                    break;
                case HIGH_PASS:
                    FilterType=NOTCH;
                    break;
                case NOTCH:
                    FilterType=BAND_PASS;
                    break;
            }
        }
        filter.setFilterType(FilterType);
    }

   
    //filter.setCutoff(fc);

    for (int ch = 0; ch<buffer.getChannels(); ++ch) {
        for (int i = 0 ; i < size; i++) {
            lfo.updateLFO_value();
            fc=fc_offset+lfo.get_LFO_value()*depth;
            if(fc<0)
            {
                fc=0;
            }
            filter.setCutoff(fc);
            
            float* buf = buffer.getSamples(ch);
            //lfo.updateLFO_value();

            float dry = buf[i]*(1-1);
            //float wet = buf[i]*fc/10000;
            float wet = filter.doFiltering(buf[i])*1;
            buf[i] = dry+wet;
        }
    }
  }
    
};


#endif /* __FlangerPatch_hpp__ */