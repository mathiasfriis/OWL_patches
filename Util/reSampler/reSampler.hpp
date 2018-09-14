#ifndef __reSampler_hpp__
#define __reSampler_hpp__

#include "CircularBuffer.hpp"

static int Ls [100] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,1,1,1,1,1,1,1,2,2,2,2,2,2,2,3,1,1,1,1,1,1,1,1,1,1,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,5,5,1,1,1,1,1,1,1,1,1,1};
static int Ms [100] = {91,46,31,23,19,16,13,12,11,10,9,8,7,7,7,6,6,6,5,5,5,5,4,4,4,4,4,7,7,7,3,3,3,3,3,3,3,5,5,5,5,5,5,5,7,2,2,2,2,2,2,2,2,2,2,5,5,5,5,5,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,5,5,5,5,5,6,6,1,1,1,1,1,1,1,1,1,1};

class reSampler
{
private:
	float currentSample;
	float inputSampleRate;
	float outputSampleRate;
	int sampleRateDivider;
	float findMultiRates(float,float);
	int L,M;
	float getDecimalSampleWithInterpolation(float*,float);
public:
	void initDownSampler();
	void downSample(float inputBuffer[], float outputBuffer[], int size, int decimationRate);
	void upSample(float inputBuffer[], float outputBuffer[], int size, int interpolationRate);
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


void reSampler::downSample(float inputBuffer[], float outputBuffer[], int size, int decimationRate)
{

	for (int i = 0 ; i < size; i++) 
	{
    	//Get downsampled signal
        if(i%decimationRate==0)
        {
        	currentSample=inputBuffer[i];
        }
        //Copy current downsampled sample to output buffer.
        inputBuffer[i]=currentSample;
    }        
 }

//Unknown if this works!
void reSampler::upSample(float inputBuffer[], float outputBuffer[], int size, int interpolationRate)
{
	//get size of inputbuffer - NOTE: SIZE OF BUFFERS MUST MATCH!
	int inputBufferSize = size;
	int outputBufferSize = inputBufferSize*interpolationRate;

	float dy_dx_over_L;

	for (int i = 0 ; i < inputBufferSize; i++) 
	{
        //Copy samples of input buffer to outputbuffer and do zero-stuffing.
        outputBuffer[i*interpolationRate]=inputBuffer[i];
        
        /*
		//create new points with linear interpolation.
        dy_dx_over_L=(inputBuffer[i+1]-inputBuffer[i])/interpolationRate;
        for (int j = 1; j< interpolationRate; j++)
        {
        	outputBuffer[i*interpolationRate+j]=inputBuffer[i]+j*dy_dx_over_L;
        }
        */
    }

    //Last L points cannot use dy/dx, and thus just copies the last known sample.
    for (int j = 1; j< interpolationRate; j++)
    {
    	outputBuffer[inputBufferSize*interpolationRate+j]=outputBuffer[inputBufferSize*interpolationRate];
    }
}


void reSampler::reSample(AudioBuffer &inputBuffer, AudioBuffer &outputBuffer, float multiRate, float multiRateMargin)
{

	//get size of inputbuffer - NOTE: SIZE OF BUFFERS MUST MATCH!
	int size = inputBuffer.getSize();

	//find L(Interpolation rate) and M(Decimation Rate)
	findMultiRates(multiRate,multiRateMargin);
	L=3;
	M=10;

	int inputBufferSize = inputBuffer.getSize();

	//create buffer to hold interpolated signal that's L(Interpolation Rate) times as big as the input buffer
	//float *InterpolatedSignalBuffer=new float[3];
	float *InterpolatedSignalBuffer = (float*)malloc(sizeof(float)*inputBufferSize*L);

	float* inputSamples = inputBuffer.getSamples(0);

	//Interpolate signal by a rate of L and save in new buffer
	upSample(inputSamples, InterpolatedSignalBuffer,size, L);

	//create buffer to hold interpolated signal that's L(Interpolation Rate)/M(Decimation Rate) times as big as the input buffer
	//float *DownsampledSignalBuffer = (float*)malloc(sizeof(float)*inputBufferSize*L/M);

	//Fake-Downsample interpolated signal by a rate of M and save in a buffer.
	downSample(InterpolatedSignalBuffer, InterpolatedSignalBuffer,size*L,M);

	//Fit downsampled buffer into outputBuffer with linear interpolation
	float* buf = outputBuffer.getSamples(0);
	float achievedMultiRate=L;
	
	for(int i=0 ; i<outputBuffer.getSize();i++)
	{
		buf[i]=InterpolatedSignalBuffer[i*L];
		//buf[i]=getDecimalSampleWithInterpolation(InterpolatedSignalBuffer,i*achievedMultiRate);
	}

	//Free up memory of InterpolatedSignalBuffer
	free(InterpolatedSignalBuffer);
	//free(DownsampledSignalBuffer);

}


float reSampler::getDecimalSampleWithInterpolation(float* buffer,float sampleNo)
{
	int wholeSample=(int)sampleNo;
	float decimal = fmod(sampleNo,1);

	float x0=buffer[wholeSample];
	float x1=buffer[wholeSample+1];

	return x0 + (x1-x0)*decimal;
}
/*
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
    */

//Find interpolation rate and decimation rate for a desired multirate with the given margin.
float reSampler::findMultiRates(float desiredRate, float margin)
    {
    	int index = (int)desiredRate*100+0.5; //Add 0.5 for proper rounding
    	M=Ms[index];
    	L=Ls[index];
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