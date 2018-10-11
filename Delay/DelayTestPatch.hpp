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
#ifndef ___DelayTestPatch_hpp__
#define __DelayTestPatch_hpp__

#include "CircularBuffer.hpp"
//#include "lfo.hpp"

#define MAX_DELAY_MS 3000
#define DEFAULT_SAMPLE_RATE 48000

class DelayTestPatch : public Patch {
private:
    //static const int MAX_DELAY_SAMPLES = MAX_DELAY_MS*DEFAULT_SAMPLE_RATE/1000;
    int MAX_DELAY_SAMPLES = MAX_DELAY_MS*getSampleRate()/1000;
    CircularBuffer* x;
    CircularBuffer* y;
    float delay_ms;
    float feedback;
    float depth;
    //float fs;
    CircularBuffer* delayBuffer;

public:
  DelayTestPatch(){
    registerParameter(PARAMETER_A, "Delay");
    registerParameter(PARAMETER_B, "Feedback");
    registerParameter(PARAMETER_C, "N/A");
    registerParameter(PARAMETER_D, "Depth");

    x = CircularBuffer::create(MAX_DELAY_SAMPLES);
    y = CircularBuffer::create(MAX_DELAY_SAMPLES);
  }

  ~DelayTestPatch() {
        CircularBuffer::destroy(x);
        CircularBuffer::destroy(y);
    }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
      
    //Read knobs
    delay_ms     = getParameterValue(PARAMETER_A)*MAX_DELAY_MS;
    feedback = getParameterValue(PARAMETER_B);
    depth    = getParameterValue(PARAMETER_D);

    //Calculate delay in samples
    int delaySamples =delay_ms*getSampleRate()/1000;

    for (int ch = 0; ch<buffer.getChannels()-1; ++ch) {
        
        float* buf = buffer.getSamples(ch);
        for (int i = 0 ; i < size; i++) {

            //Write input to x-buffer
            x->write(buf[i]);
            
            float delayedSignal = y->readDelayed(delaySamples);
            float inputSignal = buf[i];

            y->write(inputSignal+delayedSignal*feedback);

            buf[i]=inputSignal+delayedSignal*depth;
        }
    }
  }
    
};


#endif /* __DelayTestPatch_hpp__ */