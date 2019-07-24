# ifndef COUNTER_N_H
# define COUNTER_N_H

# include "../../../include/netxpto_20180815.h"

class CounterN : public Block {
public:
	//##############################################################################################################

	CounterN(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};
	CounterN(vector<Signal *> &InputSig, vector<Signal *> &OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	int n{ 100 };

	//###############################################################################################################

	void setN(int number) { n = number; };

private:
	//################################State variables################################################################
	int m{ 0 };
	unsigned int state{ 0 };
	bool reset{ false };

};

#endif // !ANDGATE_H

