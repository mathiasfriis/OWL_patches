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
#include "lfo.hpp"

#define MAX_DELAY_MS 300
#define MIN_DELAY_MS 50
#define DELAY_RANGE_MS (MAX_DELAY_MS-MIN_DELAY_MS)
#define MAX_VARIATION_MS 300
#define DEFAULT_SAMPLE_RATE 48000

class StutterPatch : public Patch {
private:
    int MAX_DELAY_SAMPLES = (MAX_DELAY_MS + MAX_VARIATION_MS)*getSampleRate()/1000;
    CircularBuffer* x;
    lfo_mode mode;
    LFO lfo;
    float period_ms;
    float variation;
    float variation_ms;
    float depth;
    int variation_samples;
    bool stutterTriggered;
    bool stutterTriggered_state;
    int index;

public:
  StutterPatch(){
    registerParameter(PARAMETER_A, "Time");
    registerParameter(PARAMETER_B, "Variation");
    registerParameter(PARAMETER_C, "N/A");
    registerParameter(PARAMETER_D, "Depth");
    registerParameter(PARAMETER_E, "Triggerer");

    x = CircularBuffer::create(MAX_DELAY_SAMPLES);
    stutterTriggered=false;
    stutterTriggered_state=false;

    lfo.setSampleRate(getSampleRate());
    lfo.setLFO_mode(sampleHold);
    lfo.setFrequency(100);
    lfo.setWaveshape(50);
  }

  ~StutterPatch() {
        CircularBuffer::destroy(x);
    }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
      
    
    //Read knobs
    period_ms     = MIN_DELAY_MS+getParameterValue(PARAMETER_A)*DELAY_RANGE_MS;
    variation = getParameterValue(PARAMETER_B);
    depth = getParameterValue(PARAMETER_D);
    //Calculate delay in samples
    int periodSamples =period_ms*getSampleRate()/1000;
    if(getParameterValue(PARAMETER_E)<0.05)
    {
        stutterTriggered=true;
    }
    else
    {
        stutterTriggered=false;
    }

     //If just triggered
    if((stutterTriggered==true) && (stutterTriggered_state==false))
    {
        //Calculate the variation in samples - LFO supplies random number between 0 and 1.
        variation_ms=lfo.get_LFO_value()*variation*MAX_VARIATION_MS;
        variation_samples=variation_ms*getSampleRate()/1000;
    }

    stutterTriggered_state=stutterTriggered;



    for (int ch = 0; ch<buffer.getChannels()-1; ++ch) {
        
        float* buf = buffer.getSamples(ch);
        for (int i = 0 ; i < size; i++) {
            lfo.updateLFO_value();
            //If not triggered, just write input to buffer, and reset index-counter.
            if(!stutterTriggered)
            {
                x->write(buf[i]); 
                index=periodSamples+variation_samples; 

                //Make sure that index stays in range
                if(index > x-> getSize())
                {
                    index = x-> getSize();
                }  
            }
            else
            {
               
                //Write to output buffer.
                buf[i]=buf[i]*(1-depth) + x->readDelayed(index)*depth;
                index--; //Decrement index counter.
                if(index<0) //If reaching head, reset counter.
                {
                    index=periodSamples+variation_samples; 
                    //Make sure that index stays in range
                    if(index > x-> getSize())
                    {
                        index = x-> getSize();
                    }  
                }
            }
        }
    }
  }
    
};


#endif /* __DelayTestPatch_hpp__ */