# ifndef PROGRAM_INCLUDE_AMPLITUDE_MODULATOR_H_
# define PROGRAM_INCLUDE_AMPLITUDE_MODULATOR_H_

# include "../include/netxpto_20180815.h"

// this is a test block for the purpose of beta testing new code
// current code: Intensity Modulator.
class AmplitudeModulator : public Block {

	bool firstTime{ true };
	double dutyCycle = .25;
	int aux = 1;

public:

	AmplitudeModulator(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};
	AmplitudeModulator(vector<Signal *> &InputSig, vector<Signal *> &OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	void setDutyCycle(double dCycle) { dutyCycle = dCycle; }
	double getDutyCycle() { return dutyCycle*100; }


};
#endif
