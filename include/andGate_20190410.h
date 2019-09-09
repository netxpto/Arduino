# ifndef ANDGATE_H
# define ANDGATE_H

# include "../include/netxpto_20180815.h"

class AndGate : public Block {
public:
	//##############################################################################################################

	AndGate(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};
	AndGate(vector<Signal *> &InputSig, vector<Signal *> &OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	//###############################################################################################################
private:
	//################################State variables################################################################
	

};

#endif // !ANDGATE_H

