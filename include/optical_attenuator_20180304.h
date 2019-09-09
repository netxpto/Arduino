#ifndef OPTICAL_ATTENUATOR_H
#define OPTICAL_ATTENUATOR_H

# include "../include/netxpto_20180815.h"
#include <math.h>

class OpticalAttenuator : public Block {

private:
	/*Input parameters*/
	double kDB = 0.0; // in dB
	double fiberLength_km{ 1.0 };

public:
	/*Methods*/
	OpticalAttenuator(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : Block(inputSignals, outputSignals) {};
	OpticalAttenuator(vector<Signal *> &inputSignals, vector<Signal *> &outputSignals) : Block(inputSignals, outputSignals) {};

	void initialize(void);
	bool runBlock(void);

	void setAttenuationCoef(double coef) { kDB = coef; };
	double getAttenuationCoef() { return kDB; }

	void setFiberLength(double fLength) { fiberLength_km = fLength; };
	double getFiberLength() { return fiberLength_km; };
};

#endif // !OPTICAL_ATTENUATOR_H
