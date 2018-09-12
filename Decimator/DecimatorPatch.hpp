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
#ifndef ___DecimatorPatch_hpp__
#define __DecimatorPatch_hpp__

#include "CircularBuffer.hpp"
#include "lfo.hpp"
#include "reSampler.hpp"
#include "Resample.h"

#define FLANGER_BUFFER_SIZE 1024
#define CUTOFF_MIN 0
#define CUTOFF_SCALER 2000
#define Q_SCALER 10
#define RATE_SCALER 20
#define DEPTH_SCALER 8000
#define MULTIRATE_MARGIN 0.05

class DecimatorPatch : public Patch {
private:
    float fs_system;
    float fs_offset;
    float LFO_rate, LFO_depth, LFO_waveshape;
    float multiRate, multiRateState;
    int L, M; //Interpolation rate and decimation rate.
    bool buttonState;
    reSampler decimator;
    LFO lfo;
    lfo_mode LFO_MODE = sine;
    

public:
  DecimatorPatch(){
    AudioBuffer* buffer = createMemoryBuffer(1, FLANGER_BUFFER_SIZE);

    registerParameter(PARAMETER_A, "LFO Rate");
    registerParameter(PARAMETER_B, "LFO Depth");
    registerParameter(PARAMETER_C, "LFO Waveshape");
    registerParameter(PARAMETER_D, "Sample Freq Offset");
   
    fs_system = getSampleRate();

    decimator.initDownSampler();
    decimator.setInputSampleRate(fs_system);
   
    lfo.initLFO();
    lfo.setSampleRate(fs_system);
    lfo.setLFO_mode(LFO_MODE);
    lfo.setWaveshape(50);
  }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
      
      LFO_rate=getParameterValue(PARAMETER_A)*RATE_SCALER;
      LFO_depth=getParameterValue(PARAMETER_B)*DEPTH_SCALER; //0:1
      LFO_waveshape=getParameterValue(PARAMETER_C)*100;
      
      lfo.setFrequency(LFO_rate);
      lfo.setWaveshape(LFO_waveshape);

      fs_offset = getParameterValue(PARAMETER_D); //0:1

      multiRateState=multiRate;
      multiRate = fs_offset + lfo.get_LFO_value()*LFO_depth;
      //multiRate = fs_offset + lfo.get_LFO_value()*LFO_depth;

/*
      if(multiRate!=multiRateState)
      {
      }
      //decimator.setOutputSampleRate();

      for(int i = 0 ; i < size; i++)
      {
        lfo.updateLFO_value();
        if(lfo.get_LFO_value()>0.99)
        {

        }
      }
*/

    if(buttonState!=isButtonPressed(PUSHBUTTON))
    {
        buttonState=isButtonPressed(PUSHBUTTON);
        if(buttonState==false)
        {
            //If button pressed
            switch(LFO_MODE)
            {
              case sine:
                LFO_MODE = square;
                break;
              case square:
                LFO_MODE = triangle;
                break;
              case triangle:
                LFO_MODE = sampleHold;
                break;
              case sampleHold:
                LFO_MODE = sine;
                break;
            }
            lfo.setLFO_mode(LFO_MODE);
        }
    }

    decimator.reSample(&AudioBuffer,&AudioBuffer,fs_offset,0.9);
    /*
    for (int ch = 0; ch<buffer.getChannels(); ++ch) {
        
        for (int i = 0 ; i < size; i++) {
            float* buf = buffer.getSamples(ch);
            main_lfo.updateLFO_value();

            float dry = buf[i]*(1-main_depth);
            float wet = buf[i]*main_lfo.get_LFO_value()*(main_depth);
            buf[i] = dry+wet;
        }
    }
    */

  }
};


#endif /* __DecimatorPatch_hpp__ */