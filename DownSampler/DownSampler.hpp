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
#ifndef ___DownSamplerPatch_hpp__
#define __DownSamplerPatch_hpp__

#include "CircularBuffer.hpp"
#include "lfo.hpp"
#include "downSampler.hpp"

#define FLANGER_BUFFER_SIZE 1024
#define CUTOFF_MIN 0
#define CUTOFF_SCALER 2000
#define Q_SCALER 10
#define RATE_SCALER 10
#define DEPTH_SCALER 8000

class AutoWahPatch : public Patch {
private:
    float fs_system;
    float fs_offset;
    bool buttonState;
    downSampler downSampler;

public:
  AutoWahPatch(){
    AudioBuffer* buffer = createMemoryBuffer(1, FLANGER_BUFFER_SIZE);

    //registerParameter(PARAMETER_A, "Rate");
    //registerParameter(PARAMETER_B, "Depth");
    //registerParameter(PARAMETER_C, "Q");
    registerParameter(PARAMETER_D, "Sample Freq Offset");
   
    fs_system = getSampleRate();

    downSampler.initDownSampler();
    downSampler.setInputSampleRate(fs_system);
   
    //lfo.initLFO();
    //lfo.setSampleRate(fs);
    //lfo.setLFO_mode(triangle);
    //lfo.setWaveshape(50);
  }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
      
      fs_offset = getParameterValue(PARAMETER_D)*fs_system;
      downSampler.setOutputSampleRate(fs_offset);

  //lfo.setFrequency(rate);

    if(buttonState!=isButtonPressed(PUSHBUTTON))
    {
        buttonState=isButtonPressed(PUSHBUTTON);
        if(buttonState==false)
        {
            //If button pressed
            //Do nothing   
        }
    }

    downSampler.downSample(buffer, buffer);

  }
    
};


#endif /* __DownSamplerPatch_hpp__ */