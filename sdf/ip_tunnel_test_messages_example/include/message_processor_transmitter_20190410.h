#ifndef MESSAGE_PROCESSOR_TRANSMITTER_H_
#define MESSAGE_PROCESSOR_TRANSMITTER_H_

#include "../../../include/netxpto_20180815.h"
#include "./message_processor_common_20190410.h"

class MessageProcessorTransmitter : public Block {
public:

	MessageProcessorTransmitter(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);

	bool runBlock(void);

	// Private methods


	// State variables
	vector<t_message> storedMessages = {};
	
	// Basis reconciliation
	bool basisReconStarted{ false };
	t_real basisReconLength{ 0 };
	vector<t_integer> basisReconVector{};
	
	// Parameter estimation
	bool ParamEstStarted{ false };
	t_real paramEstLength{ 0 };
	vector<t_integer> paramEstVector{};
	
	// Error correction - Parities
	bool ErrCorrParityStarted{ false };
	t_real ErrCorrParityLength{ 0 };
	vector<t_integer> ErrCorrParityVector{};
	
	// Error correction - Permutation seeds
	bool ErrCorrPermStarted{ false };
	t_real ErrCorrPermLength{ 0 };
	vector<t_integer> ErrCorrPermVector{};

	// Error correction - BER change signal
	bool ErrCorrBerStarted{ false };
	t_real ErrCorrBerLength{ 0 };
	vector<t_integer> ErrCorrBerVector{};

	// Privacy amplification seeds
	vector<t_integer> privacySeedsBVector{};
	t_real privacySeedsLength{ 0 };
	bool privacySeedsStarted{ false };
};
#endif // !MESSAGE_PROCESSOR_TRANSMITTER_H_
