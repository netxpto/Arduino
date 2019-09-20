# ifndef SIMPLE_FRAMER_H_
# define SIMPLE_FRAMER_H_

# include "../include/netxpto_20180815.h"

enum class frame_header_mode { Sequential };

class Framer : public Block {

public:

	//##############################################################################################################

	Framer(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);



	//##############################################################################################################

	void setFrameLength(t_integer fLength) { frameLength = fLength; };
	t_integer const getFrameLength(void) { return frameLength; };

	void setHeaderLength(t_integer hLength) { headerLength = hLength; };
	t_integer const getHeaderLength(void) { return headerLength; };

	void setHeaderSignalingLength(t_integer hSLength) { headerSignalLength = hSLength; };
	t_integer const getHeaderSignalingLength(void) { return headerSignalLength; };

	//##############################################################################################################

private:

	/* Input Parameters */
	t_integer frameLength{ 0 };
	t_integer headerLength{ 0 };
	t_integer headerSignalLength{ 0 };

	/* State Variables */
	t_integer bitPosition{ 0 };
	t_integer frameCount{ 0 };
	t_integer toAdd{ 0 };
	frame_header_mode framerMode{ frame_header_mode::Sequential };
};

# endif


