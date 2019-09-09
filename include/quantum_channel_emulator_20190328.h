#ifndef MESSAGE_QUANTUM_CHANNEL_EMU_H
#define MESSAGE_QUANTUM_CHANNEL_EMU_H

#include "../include/netxpto_20180815.h"

using namespace std;

class QuantumChannelEmulator: public Block {

public:
	/*Public Methods*/
	QuantumChannelEmulator() {};
	QuantumChannelEmulator(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	void setProbabilityOfBothClick(t_real pbc) { probabilityOfBothClick = pbc; }
	void setProbabilityOfNoClick(t_real pnc) { probabilityOfNoClick = pnc; }
	void setProbabilityOfError(t_real pe) { probabilityOfError = pe; }
private:
	/*Input Parameters*/

	/*State Variables*/
	int seed{ 971931 };
	t_real probabilityOfError{ 0};
	t_real probabilityOfBothClick{ 0 };
	t_real probabilityOfNoClick{ 0 };

	/*Private Methods*/
	UniformRandomRealNumbers<> uniformeRandomNumberBetweenZeroAndOne{ seed, 0, 1 };
};

#endif // !MESSAGE_PROCESSOR_BOB_H

