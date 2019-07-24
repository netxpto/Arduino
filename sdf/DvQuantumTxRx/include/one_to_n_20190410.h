# ifndef ONE_TO_N_H
# define ONE_TO_N_H

# include "../../../include/netxpto_20180815.h"

class OneToN : public Block {
public:
	//##############################################################################################################

	OneToN(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};
	OneToN(vector<Signal *> &InputSig, vector<Signal *> &OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	int n{ 1 };

	//###############################################################################################################

	void setN(int number) { n = number; };

private:
	//################################State variables################################################################
	

};

#endif // !ANDGATE_H

