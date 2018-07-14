#ifndef ENVELOPE_H_
#define ENVELOPE_H_

#define ENVELOPE_VALUE_DIVIDER 32000

class envelope_follower
	{
	private:
		float alpha = 0.01;
		float envelopeValue = 0;
	public:
		void updateEnvelopeValue(float input);
		float getEnvelopeValue(void);
		void setResponsiveness(float responsiveness);
	};

// Update envelope value
void envelope_follower::updateEnvelopeValue(float input)
{
	envelopeValue = alpha * abs(input) + (1.0 - alpha) * envelopeValue;
}

// Get latest envelope value
float envelope_follower::getEnvelopeValue()
{
	return envelopeValue / ENVELOPE_VALUE_DIVIDER;
}

void envelope_follower::setResponsiveness(float responsiveness)
{
	alpha=responsiveness;
}

#endif