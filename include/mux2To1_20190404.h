#ifndef MUX2TO1_H
#define MUX2TO1_H

# include "../include/netxpto_20180815.h"

class Mux2To1 : public Block {
public:
	//##############################################################################################################

	Mux2To1(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};
	Mux2To1(vector<Signal *> &InputSig, vector<Signal *> &OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################
	//###############################################################################################################
private:
	//################################State variables################################################################
	bool commutate{ true };
	bool firstTime{ true };
	t_binary sel;

};

#endif
