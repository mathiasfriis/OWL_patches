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
#ifndef ___TwinTremoloPatch_hpp__
#define __TwinTremoloPatch_hpp__

#include "CircularBuffer.hpp"
#include "lfo.hpp"

#define FLANGER_BUFFER_SIZE 1024

class TwinTremoloPatch : public Patch {
private:
    float control_rate, control_depth, main_rate, main_depth, control_waveshape, main_waveshape;
    lfo_mode control_mode, main_mode;
    LFO main_lfo;
    LFO control_lfo;
    bool buttonState;

    
public:
  TremoloPatch(){
    AudioBuffer* buffer = createMemoryBuffer(1, FLANGER_BUFFER_SIZE);
    registerParameter(PARAMETER_A, "Control Rate");
    registerParameter(PARAMETER_B, "Control Depth");
    registerParameter(PARAMETER_C, "Main Rate");
    registerParameter(PARAMETER_D, "Main Depth");
    main_lfo.initLFO();
    control_lfo.initLFO();
    float fs = getSampleRate();
    main_lfo.setSampleRate(fs);
    control_lfo.setSampleRate(fs);
  }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
    unsigned int delaySamples;
      
    control_rate     = (getParameterValue(PARAMETER_A)*30);
    control_depth    = getParameterValue(PARAMETER_B);
    main_rate = getParameterValue(PARAMETER_C)*30;
    main_depth = getParameterValue(PARAMETER_D);

    main_lfo.setWaveshape(waveshape);
    main_lfo.setFrequency(rate);
    
    if(buttonState!=isButtonPressed(PUSHBUTTON))
    {
        buttonState=isButtonPressed(PUSHBUTTON);
        if(buttonState==false)
        {
            switch(control_mode)
            {
                case sine:
                    control_waveshape=50;
                    control_mode=square;
                    break;
                case square:
                    control_mode=triangle;
                    break;
                case triangle:
                    control_waveshape=90;
                    control_mode=sine;
                    break;
            }
            control_lfo.setWaveshape(control_waveshape);
            control_lfo.setLFO_mode(control_mode);
        }
    }

    for (int ch = 0; ch<buffer.getChannels(); ++ch) {
        for (int i = 0 ; i < size; i++) {
            float* buf = buffer.getSamples(ch);
            control_lfo.updateLFO_value();
            main_lfo.setFrequency(rate+control_lfo.get_LFO_value); //Modulate main LFO rate by value gained from control LFO.
            main_lfo.updateLFO_value();
            float dry = buf[i]*(1-main_depth);
            float wet = buf[i]*main_lfo.get_LFO_value()*(main_depth);
            buf[i] = dry+wet;
        }
    }
  }
    
};


#endif /* __FlangerPatch_hpp__ */