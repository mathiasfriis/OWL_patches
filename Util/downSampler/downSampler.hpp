#ifndef __downSampler_hpp__
#define __downSampler_hpp__

#include "CircularBuffer.hpp"

class downSampler
{
private:
	float currentSample;
	float inputSampleRate;
	float outputSampleRate;
	int sampleRateDivider;

public:
	void initDownsampler();

	void downSample(AudioBuffer &buffer);
	void setInputSampleRate(float fs);
	void setOutputSampleRate(float fs);
};


void initDownsampler()
{
	currentSample=0;
	inputSampleRate=44100;
	outputSampleRate=44100;
	sampleRateDivider =1;
}


void downSampler::downSample(AudioBuffer &inputBuffer, AudioBuffer &outputBuffer)
{
	//get size of inputbuffer - NOTE: SIZE OF BUFFERS MUST MATCH!
	int size = inputBuffer.getSize();

	for (int ch = 0; ch<inputBuffer.getChannels(); ++ch) {
        for (int i = 0 ; i < size; i++) {

        	//Get downsampled signal
            if(i%sampleRateDivider==0)
            {
            	currentSample=inputBuffer[i];
            }
            //Copy current downsampled sample to output buffer.
            outputBuffer[i]=currentSample;
        }
    }
}

void downSampler:: setInputSampleRate(float fs)
{
	inputSampleRate=fs;
}

void downSampler::setOutputSampleRate(float fs)
{
	outputSampleRate=fs;
	sampleRateDivider=ceil(inputSampleRate/outputSampleRate);
}

#endif // __downSampler_hpp__