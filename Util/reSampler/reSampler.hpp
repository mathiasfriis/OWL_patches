#ifndef __reSampler_hpp__
#define __reSampler_hpp__

#include "CircularBuffer.hpp"

class reSampler
{
private:
	float currentSample;
	float inputSampleRate;
	float outputSampleRate;
	int sampleRateDivider;
	float findMultiRates(float,float);
	int L,M;
public:
	void initDownSampler();
	void downSample(AudioBuffer &inputBuffer, AudioBuffer &outputBuffer, int decimationRate);
	void upSample(AudioBuffer &inputbuffer, float outputBuffer[], int interpolationRate);
	void reSample(AudioBuffer &inputbuffer, AudioBuffer &outputBuffer, float multiRate, float multiRateMargin);
	void setInputSampleRate(float fs);
	void setOutputSampleRate(float fs);
};


void reSampler::initDownSampler()
{
	currentSample=0;
	inputSampleRate=44100;
	outputSampleRate=44100;
	sampleRateDivider =1;
}


void reSampler::downSample(AudioBuffer &inputBuffer, AudioBuffer &outputBuffer, int decimationRate)
{
	//get size of inputbuffer - NOTE: SIZE OF BUFFERS MUST MATCH!
	int size = inputBuffer.getSize();



	for (int ch = 0; ch<inputBuffer.getChannels(); ++ch) {
		float* inputBuf = inputBuffer.getSamples(ch);
		float* outputBuf = outputBuffer.getSamples(ch);
        for (int i = 0 ; i < size; i++) {

        	//Get downsampled signal
            if(i%decimationRate==0)
            {
            	currentSample=inputBuf[i];
            }
            //Copy current downsampled sample to output buffer.
            outputBuf[i]=currentSample;
        }
    }
}

//Unknown if this works!
void reSampler::upSample(AudioBuffer &inputBuffer, float outputBuffer[], int interpolationRate)
{
	//get size of inputbuffer - NOTE: SIZE OF BUFFERS MUST MATCH!
	int inputBufferSize = inputBuffer.getSize();
	int outputBufferSize = inputBufferSize*interpolationRate;


	for (int ch = 0; ch<inputBuffer.getChannels(); ++ch) {
		float* inputBuf = inputBuffer.getSamples(ch);

		float dy_dx_over_L;

        for (int i = 0 ; i < inputBufferSize; i++) {
            //Copy samples of input buffer to outputbuffer and do zero-stuffing.
            outputBuffer[i*interpolationRate]=inputBuf[i];
            
			//create new points with linear interpolation.
            dy_dx_over_L=(inputBuf[i+1]-inputBuf[i])/interpolationRate;
            for (int j = 1; j< interpolationRate; j++)
            {
            	outputBuffer[i*interpolationRate+j]=inputBuf[i]+j*dy_dx_over_L;
            }
        }
        //Last L points cannot use dy/dx, and thus just copies the last known sample.
        for (int j = 1; j< interpolationRate; j++)
        {
        	outputBuffer[inputBufferSize*interpolationRate+j]=outputBuffer[inputBufferSize*interpolationRate];
        }
        
    }
}

void reSampler::reSample(AudioBuffer &inputBuffer, AudioBuffer &outputBuffer, float multiRate, float multiRateMargin)
{
	findMultiRates(multiRate,multiRateMargin);

	int inputBufferSize = inputBuffer.getSize();

	//create buffer to hold interpolated signal that's L(Interpolation Rate) times as big as the input buffer
	float *InterpolatedSignalBuffer=new float[inputBufferSize*L];
	//*InterpolatedSignalBuffer = malloc(sizeof(float)*inputBufferSize*L);

	//Interpolate signal by a rate of L and save in new buffer
	upSample(*inputBuffer, &InterpolatedSignalBuffer, L);

	//Downsample interpolated signal by a rate of M and save in outputBuffer.

	//Free up memory of InterpolatedSignalBuffer


}

//Find interpolation rate and decimation rate for a desired multirate with the given margin.
float reSampler::findMultiRates(float desiredRate, float margin)
    {
    	float achievedRate;
    	for(int l=1;l<100;l++)
    	{
    		for(int m=1;m<100;m++)
	    	{
	    		achievedRate=l/m;
	    		if((abs(achievedRate-desiredRate)/desiredRate)<margin)
	    		{
	    			L=l;
	    			M=m;
	    			return achievedRate;
	    		}
	    	}
    	}
    }


void reSampler::setInputSampleRate(float fs)
{
	inputSampleRate=fs;
}

void reSampler::setOutputSampleRate(float fs)
{
	outputSampleRate=fs;
	sampleRateDivider=ceil(inputSampleRate/outputSampleRate);
}

#endif // __downSampler_hpp__