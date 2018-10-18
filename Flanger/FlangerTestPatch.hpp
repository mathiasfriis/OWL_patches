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
#ifndef ___FlangerTestPatch_hpp__
#define __FlangerTestPatch_hpp__

#include "CircularBuffer.hpp"
#include "lfo.hpp"

#define MAX_DELAY_MS 30
#define DEFAULT_SAMPLE_RATE 48000
#define MAX_LFO_RATE 15

enum modulator_mode{flanger,chorus};

class FlangerTestPatch : public Patch {
private:
    //static const int MAX_DELAY_SAMPLES = MAX_DELAY_MS*DEFAULT_SAMPLE_RATE/1000;
    int MAX_DELAY_SAMPLES = MAX_DELAY_MS*getSampleRate()/1000;
    CircularBuffer* y;
    float delay_ms;
    float feedback;
    float depth; //modulation depth
    float mix; //dry wet mix
    modulator_mode mod_mode;
    lfo_mode LFO_mode;
    LFO lfo;
    float lfo_freq;
    bool buttonState, ExpressionPedalTriggered, ExpressionPedalTriggered_state;
    //float fs;
    CircularBuffer* delayBuffer;

    void changeModulationMode();
    void changeLFOMode();

public:
  FlangerTestPatch(){
    registerParameter(PARAMETER_A, "Rate");
    registerParameter(PARAMETER_B, "Feedback");
    registerParameter(PARAMETER_C, "Depth");
    registerParameter(PARAMETER_D, "Mix");
    y = CircularBuffer::create(MAX_DELAY_SAMPLES);
    mod_mode = flanger;
    LFO_mode = sine;

    lfo.initLFO();
    float fs = getSampleRate();
    lfo.setSampleRate(fs);
    lfo.setWaveshape(50);
    lfo.setLFO_mode(LFO_mode);

    ExpressionPedalTriggered=false;
    ExpressionPedalTriggered_state=false;
  }

  ~FlangerTestPatch() {
        CircularBuffer::destroy(y);
    }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();

    //Manage button push - If pushed, change LFO mode
    if(buttonState!=isButtonPressed(PUSHBUTTON))
    {
        buttonState=isButtonPressed(PUSHBUTTON);
        if(buttonState==false)
        {
            //If button pressed
            switch(LFO_mode)
            {
              case sine:
                LFO_mode = square;
                break;
              case square:
                LFO_mode = triangle;
                break;
              case triangle:
                LFO_mode = sampleHold;
                break;
              case sampleHold:
                LFO_mode = sine;
                break;
            }
            lfo.setLFO_mode(LFO_mode);
        }
    }

    if(getParameterValue(PARAMETER_E)<0.05)
    {
        ExpressionPedalTriggered=true;
    }
    else
    {
        ExpressionPedalTriggered=false;
    }

     //If just triggered
    if((ExpressionPedalTriggered==true) && (ExpressionPedalTriggered_state==false))
    {
        //Change modulation mode
        switch(mod_mode)
        {
            case flanger:
                mod_mode=chorus;
                break;
            case chorus:
                mod_mode=flanger;
                break;
        }
    }

    ExpressionPedalTriggered_state=ExpressionPedalTriggered;

    lfo_freq = getParameterValue(PARAMETER_A)*MAX_LFO_RATE;
    feedback = getParameterValue(PARAMETER_B);
    depth    = getParameterValue(PARAMETER_C);
    mix    = getParameterValue(PARAMETER_D);

    int delaySamples;    

    lfo.setFrequency(lfo_freq);

    for (int ch = 0; ch<buffer.getChannels()-1; ++ch) {
        for (int i = 0 ; i < size; i++) {
            float* buf = buffer.getSamples(ch);

            //update LFO
            lfo.updateLFO_value();

            //Calculate delay in ms
            switch(mod_mode)
            {
                case flanger:
                    delay_ms     = lfo.get_LFO_value()*depth*MAX_DELAY_MS/5; //0-6ms
                    break;
                case chorus:
                    delay_ms     = lfo.get_LFO_value()*depth*MAX_DELAY_MS; //0-30ms
                    break;
            }

            //Make sure delay stays in range
            if(delay_ms>MAX_DELAY_MS)
            {
                delay_ms=MAX_DELAY_MS;
            }

            //Calculate delay in samples
            delaySamples =delay_ms*getSampleRate()/1000;

           //Seperate delay line and input signal
            float delayedSignal = y->readDelayed(delaySamples);
            float inputSignal = buf[i];

            //write the feedback to the output delay line.
            y->write(inputSignal+delayedSignal*feedback);

            //Write to output buffer.
            buf[i]=inputSignal*(1-mix)+delayedSignal*mix;
        }
    }
  }
};


#endif /* __FlangerTestPatch_hpp__ */