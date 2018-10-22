#ifndef ENVELOPE_H_
#define ENVELOPE_H_

#define ENVELOPE_VALUE_DIVIDER 1

class envelopeFollower
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
void envelopeFollower::updateEnvelopeValue(float input)
{
	envelopeValue = alpha * abs(input) + (1.0 - alpha) * envelopeValue;
}

// Get latest envelope value
float envelopeFollower::getEnvelopeValue()
{
	return envelopeValue / ENVELOPE_VALUE_DIVIDER;
}

void envelopeFollower::setResponsiveness(float responsiveness)
{
	alpha=responsiveness;
}

#endif