#ifndef MESSAGE_CLICK_ERROR_EMU_H
#define MESSAGE_CLICK_ERROR_EMU_H
#include "../include/netxpto_20180815.h"

using namespace std;

class ClickErrorEmulator : public Block {

public:
	/*Public Methods*/
	ClickErrorEmulator() {};
	ClickErrorEmulator(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

private:
	/*Input Parameters*/

	/*State Variables*/
	int seed{ 971931 };
	t_real probabilityOfTwoClicks{ 0 };
	t_real probabilityOfNoClicks{ 0 };

	/*Private Methods*/
	UniformRandomRealNumbers<> uniformeRandomNumberBetweenZeroAndOne{ seed, 0, 1 };
};

#endif // !MESSAGE_PROCESSOR_BOB_H

