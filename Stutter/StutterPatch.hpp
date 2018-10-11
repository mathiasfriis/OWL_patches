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
#ifndef ___StutterPatch_hpp__
#define __StutterPatch_hpp__

#include "CircularBuffer.hpp"
//#include "lfo.hpp"

#define MAX_DELAY_MS 500
#define DEFAULT_SAMPLE_RATE 48000

class StutterPatch : public Patch {
private:
    int MAX_DELAY_SAMPLES = MAX_DELAY_MS*getSampleRate()/1000;
    CircularBuffer* x;
    float period_ms;
    bool stutterTriggered;
    int index;

public:
  StutterPatch(){
    registerParameter(PARAMETER_A, "Time");
    registerParameter(PARAMETER_B, "N/A");
    registerParameter(PARAMETER_C, "N/A");
    registerParameter(PARAMETER_D, "N/A");
    registerParameter(PARAMETER_E, "Triggerer");

    x = CircularBuffer::create(MAX_DELAY_SAMPLES);
    stutterTriggered=false;
  }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
      
    //Read knobs
    period_ms     = getParameterValue(PARAMETER_A)*MAX_DELAY_MS;
    //Calculate delay in samples
    int periodSamples =period_ms*getSampleRate()/1000;
    if(getParameterValue(PARAMETER_E)<0.5)
    {
        stutterTriggered=true;
    }
    else
    {
        stutterTriggered=false;
    }

    for (int ch = 0; ch<buffer.getChannels()-1; ++ch) {
        
        float* buf = buffer.getSamples(ch);
        for (int i = 0 ; i < size; i++) {

            //If not triggered, just write input to buffer, and reset index-counter.
            if(!stutterTriggered)
            {
                x->write(buf[i]); 
                index=periodSamples;   
            }
            else
            {
                //Write to output buffer.
                buf[i]=x->readDelayed(index);
                index--; //Decrement index counter.
                if(index<0) //If reaching head, reset counter.
                {
                    index=periodSamples; 
                }
            }
        }
    }
  }
    
};


#endif /* __DelayTestPatch_hpp__ */