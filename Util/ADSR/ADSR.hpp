#ifndef ADSR_H_
#define ADSR_H_

typedef enum
{
	UNTRIGGERED = 0,
	IN_ATTACK = 1,
	IN_DECAY = 2,
	IN_SUSTAIN = 3,
	IN_RELEASE = 4
} ADSR_STATUS;

class ADSR
	{
	private:
		float envelopeValue;
		float A, D, S, R;
		float fs;
		bool AttackOngoing, DecayOngoing, SustainOngoing, ReleaseOngoing;
		bool TriggerHeldDown, TriggerHeldDownState;
		ADSR_STATUS status;
	public:
		ADSR(float);
		void updateValue();
		float getValue(void);
		void setAttack(float);
		void setDecay(float);
		void setSustain(float);
		void setRelease(float);
		void setTriggerHeldDown(bool);
	};


ADSR(float sampleFrequency)
{
	A=1;
	D=1;
	S=0.5;
	R=1;

	fs=sampleFrequency;

	envelopeValue = 0;

	TriggerHeldDown=false;
	TriggerHeldDownState=false;
}

// Update envelope value
void ADSR::updateValue(float input)
{
	//Check for change in trigger state
	if(TriggerHeldDown!=TriggerHeldDownState)
	{
		//Trigger just activated
		if(TriggerHeldDown)
		{
			status=IN_ATTACK;
		}

		//Trigger just released
		else
		{
			status=IN_RELEASE;
		}
	}
	switch(status)
	{
		case UNTRIGGERED:
			envelopeValue=0;
			break;
		case IN_ATTACK:
			envelopeValue+=1/(A*fs); //Increment envelope value
			if(envelopeValue>=1) //If max value reached, set in decay mode, and make sure value doesn't go above 1
			{
				envelopeValue=1;
				status=IN_DECAY;
			}
			break;
		case IN_DECAY:
			envelopeValue-=1/(D*fs); //Increment envelope value
			if(envelopeValue<=S) //If sustain value reached, set in sustain mode, and make sure value doesn't go below sustain value
			{
				envelopeValue=S;
				status=IN_SUSTAIN;
			}
			break;
		case IN_SUSTAIN:
			//Do nothing
			break;
		case IN_RELEASE:
			envelopeValue-=1/(R*fs); //Increment envelope value
			if(envelopeValue<=0) //If 0 value reached, set in untriggered mode, and make sure value doesn't go below 0;
			{
				envelopeValue=0;
				status=UNTRIGGERED;
			}
			break;
	}
	//Update state
	TriggerHeldDownState=TriggerHeldDown;
}

void ADSR::setTriggerHeldDown(bool status)
{
	TriggerHeldDown=status;
}

// Get latest envelope value
float ADSR::getValue()
{
	return envelopeValue;
}

void ADSR::setAttack(float value)
{
	//Only update value if no current cycle is ongoing
	if(status==UNTRIGGERED)
	{
		A=value;
	}
}

void ADSR::setDecay(float value)
{
	//Only update value if no current cycle is ongoing
	if(status==UNTRIGGERED)
	{
		D=value;
	}
}

void ADSR::setSustain(float value)
{
	//Only update value if no current cycle is ongoing
	if(status==UNTRIGGERED)
	{
		S=value;
	}
}

void ADSR::setRelease(float value)
{
	//Only update value if no current cycle is ongoing
	if(status==UNTRIGGERED)
	{
		R=value;
	}
}

#endif