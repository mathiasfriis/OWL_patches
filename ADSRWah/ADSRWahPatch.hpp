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
#ifndef ___ADSRWahPatch_hpp__
#define __ADSRWahPatch_hpp__

#include "ADSR.hpp"
#include "SVF.hpp"

#define CUTOFF_MIN 0
#define CUTOFF_SCALER 10000
#define Q_SCALER 10
#define A_SCALER 1
#define D_SCALER 1
#define S_SCALER 1
#define R_SCALER 1

class ADSRWahPatch : public Patch {
private:
    float fs,A,D,S,R, Q, fc, fc_offset, mix, depth;
    ADSR eg;
    StateVariableFilter filter;
    SVF_FILTER_TYPE FilterType;
    bool buttonState;
    bool ADSR_triggered, ADSR_triggered_state;

    
public:
  ADSRWahPatch(){
    //AudioBuffer* buffer = createMemoryBuffer(1, FLANGER_BUFFER_SIZE);
    registerParameter(PARAMETER_A, "A");
    registerParameter(PARAMETER_B, "D");
    registerParameter(PARAMETER_C, "S");
    registerParameter(PARAMETER_D, "Depth"); 
    registerParameter(PARAMETER_E, "ExpressionTrigger"); 
    fs = getSampleRate();
    filter.initFilter(fs);
    fc=0;
    FilterType = BAND_PASS;
    filter.setFilterType(FilterType);
    mix=1;

    eg.setSampleFrequency(fs);

    ADSR_triggered=false;
    ADSR_triggered_state=false;

    fc_offset=0;
  }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
      
    A     = getParameterValue(PARAMETER_A)*A_SCALER;
    D = getParameterValue(PARAMETER_B)*D_SCALER; // so we keep a -3dB summation of the delayed signal
    S = getParameterValue(PARAMETER_C)*S_SCALER;
    R = 0*R_SCALER; //Instant release

    depth = getParameterValue(PARAMETER_D)*2-1; //-1:1

    eg.setAttack(A);
    eg.setDecay(D);
    eg.setSustain(S);
    eg.setRelease(R);

    filter.setQfactor(0.5*Q);

    //Check if ExpressionTriggerer has been pressed - set ASDF-trigger accordingly;
    if(getParameterValue(PARAMETER_E)<0.05)
    {
        ADSR_triggered=true;
    }
    else
    {
        ADSR_triggered=false;
    }

    eg.setTriggerHeldDown(ADSR_triggered);

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

    for (int ch = 0; ch<buffer.getChannels()-1; ++ch) {
    	float* buf = buffer.getSamples(ch);
        for (int i = 0 ; i < size-1; i++) {
            eg.updateValue();
            fc=fc_offset+eg.getValue()*CUTOFF_SCALER*depth;
            if(fc<0)
            {
                fc=0;
            }
            if(fc>(fs/2-1))
            {
                fc=fs/2-1;
            }
            filter.setCutoff(fc);
            fc=500;
            float dry = buf[i]*(1-mix);
            //float wet = buf[i]*fc/10000;
            float wet = filter.doFiltering(buf[i])*mix;
            buf[i] = dry+wet;
        }
    }
  }
    
};


#endif /* __ADSRWahPatch_hpp__ */