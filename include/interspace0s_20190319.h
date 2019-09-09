# ifndef INTERSPACE0S_H_
# define INTERSPACE0S_H_

# include "../include/netxpto_20180815.h"

class Interspace0s : public Block {

public:

	//################################################################################################################################

	Interspace0s(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//################################################################################################################################
		
	void setUpsamplingFactor(int uFactor) { upSamplingFactor = uFactor; }
	int const getUpsamplingFactor(void) { return upSamplingFactor; };

	//################################################################################################################################

private:

	/* input parameters */
	int upSamplingFactor{ 8 };
	int index{ 0 };

};

#endif
