#ifndef OPTICAL_ATTENUATOR_H
#define OPTICAL_ATTENUATOR_H

#include "netxpto_20180815.h"
#include <math.h>

class OpticalAttenuator : public Block {

private:
	/*Input parameters*/
	double kDB = 0.0; // in dB

public:
	/*Methods*/
	OpticalAttenuator() {};
	OpticalAttenuator(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	void setAttenuationCoef(double coef) { kDB = coef; };
	double getAttenuationCoef() { return kDB; }
};

#endif // !OPTICAL_ATTENUATOR_H
