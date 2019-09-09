# ifndef ADD_H_
# define ADD_H_

# include "netxpto_20190816.h"


class Add : public Block {

public:

	//##############################################################################################################

	Add(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

private:

	bool firstTime{ true };

};

#endif
