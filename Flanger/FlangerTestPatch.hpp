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
#define MIN_DELAY_MS 1
#define DEFAULT_SAMPLE_RATE 48000
#define MAX_LFO_RATE 15

enum modulator_mode{flanger,chorus};

class FlangerTestPatch : public Patch {
private:
    //static const int MAX_DELAY_SAMPLES = MAX_DELAY_MS*DEFAULT_SAMPLE_RATE/1000;
    int MAX_DELAY_SAMPLES = (MAX_DELAY_MS+MIN_DELAY_MS)*getSampleRate()/1000;
    CircularBuffer* output_L;
    CircularBuffer* output_R;
    float delay_ms;
    float feedback;
    float depth; //modulation depth
    float mix; //dry wet mix
    modulator_mode mod_mode;
    lfo_mode LFO_mode;
    LFO lfo;
    float lfo_freq;
    bool buttonState, ExpressionPedalTriggered, ExpressionPedalTriggered_state;

    void changeModulationMode();
    void changeLFOMode();

public:
  FlangerTestPatch(){
    registerParameter(PARAMETER_A, "Rate");
    registerParameter(PARAMETER_B, "Feedback");
    registerParameter(PARAMETER_C, "Depth");
    registerParameter(PARAMETER_D, "Mix");
    output_L = CircularBuffer::create(MAX_DELAY_SAMPLES);
    output_R = CircularBuffer::create(MAX_DELAY_SAMPLES);
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
        CircularBuffer::destroy(output_L);
        CircularBuffer::destroy(output_R);
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
                LFO_mode = walk;
                break;
              case walk:
                LFO_mode = sine;
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
    feedback = (getParameterValue(PARAMETER_B)-0.5)*2;
    depth    = getParameterValue(PARAMETER_C);
    mix    = getParameterValue(PARAMETER_D);

    int delaySamples;    

    lfo.setFrequency(lfo_freq);

    float* buf_L = buffer.getSamples(0);
    float* buf_R = buffer.getSamples(1);

    for (int i = 0 ; i < size; i++) {
            float* buf = buffer.getSamples(ch);

            //update LFO
            lfo.updateLFO_value();

            //Calculate delay in ms
            switch(mod_mode)
            {
                case flanger:
                    delay_ms     = lfo.get_LFO_value()*depth*MAX_DELAY_MS/5+MIN_DELAY_MS; //1-7ms
                    break;
                case chorus:
                    delay_ms     = lfo.get_LFO_value()*depth*MAX_DELAY_MS+MIN_DELAY_MS; //1-31ms
                    break;
            }

            //Make sure delay stays in range
            if(delay_ms>MAX_DELAY_MS)
            {
                delay_ms=MAX_DELAY_MS;
            }

            if(delay_ms<MIN_DELAY_MS)
            {
                delay_ms=MIN_DELAY_MS;
            }

            //Calculate delay in samples
            delaySamples =delay_ms*getSampleRate()/1000;

           //Seperate delay line and input signal
            float delayedSignal_L = output_L->readDelayed(delaySamples);
            float inputSignal_L = buf_L[i];

            float delayedSignal_R = output_R->readDelayed(delaySamples);
            float inputSignal_R = buf_R[i];

            //write the feedback to the output delay line.
            output_L->write(inputSignal+delayedSignal*feedback);
            output_R->write(inputSignal+delayedSignal*feedback);

            //Write to output buffer.
            buf_L[i]=inputSignal_L*(1-mix)+delayedSignal_L*mix;
            buf_R[i]=inputSignal_R*(1-mix)+delayedSignal_L*mix;
        }


  }
};


#endif /* __FlangerTestPatch_hpp__ */