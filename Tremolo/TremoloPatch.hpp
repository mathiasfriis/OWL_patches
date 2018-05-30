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


#ifndef ___TremoloPatch_hpp__
#define __TremoloPatch_hpp__

#include "CircularBuffer.hpp"
#include "lfo.hpp"

#define FLANGER_BUFFER_SIZE 1024

class TremoloPatch : public Patch {
private:
    CircularBuffer delayBuffer;
    float rate, depth, feedback, phase;
    LFO lfo;
    
public:
  TremoloPatch(){
    AudioBuffer* buffer = createMemoryBuffer(1, FLANGER_BUFFER_SIZE);
    delayBuffer.initialise(buffer->getSamples(0), buffer->getSize());
    registerParameter(PARAMETER_A, "Rate");
    registerParameter(PARAMETER_B, "Depth");
    registerParameter(PARAMETER_C, "Feedback");
    registerParameter(PARAMETER_D, "");    
    phase = 0;
    lfo.initLFO();
  }
  float modulate(float rate) {
    phase += rate;
    if ( phase >= 1.0 ) {
        phase -= 1.0;
    }
    return sinf(phase*(2*M_PI));    //sine function
  };
  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
    unsigned int delaySamples;
      
    rate     = getParameterValue(PARAMETER_A) * 0.000005f; // flanger needs slow rate
    depth    = getParameterValue(PARAMETER_B);
    feedback = getParameterValue(PARAMETER_C)* 0.707; // so we keep a -3dB summation of the delayed signal
      
    for (int ch = 0; ch<buffer.getChannels(); ++ch) {
        for (int i = 0 ; i < size; i++) {
            float* buf = buffer.getSamples(ch);
            delaySamples = (depth * modulate(rate)) * (delayBuffer.getSize()-1); // compute delay according to rate and depth
            buf[i] += feedback * delayBuffer.read(delaySamples); // add scaled delayed signal to dry signal
            delayBuffer.write(buf[i]); // update delay buffer
        }
    }
  }
    
};


#endif /* __FlangerPatch_hpp__ */

////////////////////////////////////////////////////////////////////////////////////////////////////
© 2018 GitHub, Inc.
Terms
Privacy
Security
Status
Help
Contact GitHub
API
Training
Shop
Blog
About
Press h to open a hovercard with more details.