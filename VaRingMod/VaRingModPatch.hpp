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
#ifndef ___VaRingModPatch_hpp__
#define __VaRingModPatch_hpp__

#include "CircularBuffer.hpp"
#include "lfo.hpp"

#define BUFFER_SIZE 1024
#define LFO_MAX_RATE 10000 //Hz
#define LFO_MIN_RATE 30 //Hz

#define CONTROL_LFO_MAX_RATE 15 //Hz

class VaRingModPatch : public Patch {
private:
    float control_rate, control_depth, main_rate, main_depth, control_waveshape, main_waveshape;
    lfo_mode control_mode, main_mode;
    LFO main_lfo;
    LFO control_lfo;
    bool buttonState;

    
public:
  VaRingModPatch(){
    AudioBuffer* buffer = createMemoryBuffer(1, BUFFER_SIZE);
    registerParameter(PARAMETER_A, "Control Rate");
    registerParameter(PARAMETER_B, "Control Depth");
    registerParameter(PARAMETER_C, "Main Rate");
    registerParameter(PARAMETER_D, "Main Depth");
    main_lfo.initLFO();
    control_lfo.initLFO();
    float fs = getSampleRate();
    main_lfo.setSampleRate(fs);
    control_lfo.setSampleRate(fs);

    control_waveshape=sine;
    main_waveshape=sine;

    control_lfo.setLFO_mode(control_mode);
  }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
    unsigned int delaySamples;
      
    control_rate     = (getParameterValue(PARAMETER_A)*CONTROL_LFO_MAX_RATE);
    control_depth    = getParameterValue(PARAMETER_B);
    main_rate = LFO_MIN_RATE + getParameterValue(PARAMETER_C)*LFO_MAX_RATE;
    main_depth = getParameterValue(PARAMETER_D);

    main_lfo.setWaveshape(main_waveshape);
    main_lfo.setFrequency(main_rate);

    control_lfo.setWaveshape(control_waveshape);

    control_lfo.setFrequency(control_rate);
    
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

    for(int n= 0; n<size;n++)
    {
        control_lfo.updateLFO_value();    
    }
    main_lfo.setFrequency(main_rate+control_lfo.get_LFO_value()*LFO_MAX_RATE*control_depth); //Modulate main LFO rate by value gained from control LFO.


    float* buf_L = buffer.getSamples(0);
    float* buf_R = buffer.getSamples(1);

    for (int i = 0 ; i < size; i++) 
    {
        main_lfo.updateLFO_value();

        float dry_L = buf_L[i]*(1-main_depth);
        float wet_L = buf_L[i]*main_lfo.get_LFO_value()*(main_depth);
        buf_L[i] = dry_L+wet_L;

        float dry_R = buf_R[i]*(1-main_depth);
        float wet_R = buf_R[i]*main_lfo.get_LFO_value()*(main_depth);
        buf_R[i] = dry_R+wet_R;
    }
  }
    
};


#endif /* __FlangerPatch_hpp__ */