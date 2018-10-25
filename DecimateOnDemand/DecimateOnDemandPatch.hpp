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
#ifndef ___DecimateOnDemandPatch_hpp__
#define __DecimateOnDemandPatch_hpp__

#include "ADSR.hpp"
#include "reSampler.hpp"

#define FLANGER_BUFFER_SIZE 1024

#define ATTACK_SCALER 2
#define DECAY_SCALER 2
#define RELEASE_SCALER 2
#define DEPTH_SCALER 1
#define FS_OFFSET 0.5

class DecimateOnDemandPatch : public Patch {
private:
    float fs_system;
    float fs_offset;
    float A,D,S,R;
    float depth;
    float relativeSampleRate, multiRateState;
    bool ADSR_triggered;
    reSampler decimator;
    ADSR eg;
    

public:
  DecimateOnDemandPatch(){
    registerParameter(PARAMETER_A, "Attack");
    registerParameter(PARAMETER_B, "Decay");
    registerParameter(PARAMETER_C, "Release");
    registerParameter(PARAMETER_D, "Depth");
    registerParameter(PARAMETER_E, "ExpressionTrigger"); 
   
    fs_system = getSampleRate();

    decimator.initDownSampler(getBlockSize());

    decimator.setInputSampleRate(fs_system);
   
    eg.setSampleFrequency(fs_system);
  }
 

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
      
	  
   	//read parameters from knobs
    A=getParameterValue(PARAMETER_A)*ATTACK_SCALER;
    D=getParameterValue(PARAMETER_B)*DECAY_SCALER; //0:1
    R=getParameterValue(PARAMETER_C)*RELEASE_SCALER;
    depth = getParameterValue(PARAMETER_D)*DEPTH_SCALER; //0:1

    //Update ADSR
    eg.setAttack(A);
    eg.setDecay(D);
    eg.setSustain(0);
    eg.setRelease(R);

    //Check if ExpressionTriggerer has been pressed - set ASDF-trigger accordingly;
    if(getParameterValue(PARAMETER_E)<0.05)
    {
        ADSR_triggered=true;
    }
    else
    {
        ADSR_triggered=false;
    }
    eg.setTriggerHeldDown(ADSR_triggered);

    for(int i=0; i<size;i++)
    {
    	eg.updateValue();
    }

    if(eg.getADSR_Status()==UNTRIGGERED)
    {
    	relativeSampleRate = 1;
    }
    else
    {
    	//calculate relative sample rate
   		relativeSampleRate = 1 - FS_OFFSET - eg.getValue()*depth;
    }
    
     
    //Make sure relative sample rate stays between 0:1
    if(relativeSampleRate>1)
    {
    	relativeSampleRate=1;
    }
    if(relativeSampleRate<0)
    {
    	relativeSampleRate=0;
    }

    //resample signal
    decimator.reSample(buffer,buffer,relativeSampleRate,0.1);
  }
};


#endif /* __DecimateOnDemandPatch_hpp__ */