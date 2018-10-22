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

#include "envelopeFollower.hpp"
#include "SVF.hpp"

#define CUTOFF_MIN 0
#define CUTOFF_SCALER 2000
#define Q_SCALER 10
#define EG_TO_CUTOFF_SCALER 1000

class AutoWahPatch : public Patch {
private:
    float sensitivity, Q, fc, fc_offset, mix;
    envelopeFollower ef;
    StateVariableFilter filter;
    SVF_FILTER_TYPE FilterType;
    bool buttonState;

    
public:
  AutoWahPatch(){
    //AudioBuffer* buffer = createMemoryBuffer(1, FLANGER_BUFFER_SIZE);
    registerParameter(PARAMETER_A, "Sensitivity");
    registerParameter(PARAMETER_B, "Q");
    registerParameter(PARAMETER_C, "Cutoff");
    registerParameter(PARAMETER_D, "Mix"); 
    float fs = getSampleRate();
    filter.initFilter(fs);
    fc=0;
    FilterType = BAND_PASS;
    filter.setFilterType(FilterType);
  }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
      
    sensitivity     = getParameterValue(PARAMETER_A);
    Q = getParameterValue(PARAMETER_B)*Q_SCALER; // so we keep a -3dB summation of the delayed signal
    fc_offset= getParameterValue(PARAMETER_C)*CUTOFF_SCALER+CUTOFF_MIN;
    mix = getParameterValue(PARAMETER_D);
    
    filter.setQfactor(Q);
    ef.setResponsiveness(sensitivity);

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

    for (int ch = 0; ch<buffer.getChannels(); ++ch) {
    	float* buf = buffer.getSamples(ch);
        for (int i = 0 ; i < size; i++) {
            ef.updateEnvelopeValue(buf[i]);
            fc=fc_offset+ef.getEnvelopeValue()*EG_TO_CUTOFF_SCALER;
            if(fc<0)
            {
                fc=0;
            }
            filter.setCutoff(fc);

            float dry = buf[i]*(1-mix);
            //float wet = buf[i]*fc/10000;
            float wet = filter.doFiltering(buf[i])*mix;
            buf[i] = dry+wet;
        }
    }
  }
    
};


#endif /* __FlangerPatch_hpp__ */