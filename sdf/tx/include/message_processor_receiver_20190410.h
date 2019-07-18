#ifndef MESSAGE_PROCESSOR_RECEIVER_H_
#define MESSAGE_PROCESSOR_RECEIVER_H_

#include "../../../include/netxpto_20180815.h"
#include "./message_processor_common_20190410.h"

class MessageProcessorReceiver : public Block {
public:
	MessageProcessorReceiver(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);

	bool runBlock(void);

private:
	
	// Private methods
//	t_message_data getMessageDataBasisReconciliation(const t_message& msg, t_message_data_length dataLength);
//	t_message_data_length getMessageDataLength(const t_message& msg);
//	t_message_type getMessageType(const t_message& msg);

	// State variables
	vector<t_message> storedMessages{};
	
	// Basis Reconciliation
	vector<t_integer> basisIn{};
	bool basisReconRcvStarted{ false };

	// Parameter Estimation
	vector<t_integer> paramEstIn{};
	bool paramEstRcvStarted{ false };

	// Error correction - Parities
	vector<t_integer> parityIn{};
	bool errCorrParitiesStarted{ false };

	// Error correction - Permutations
	vector<t_integer> permutationsIn{};
	bool errCorrPermStarted{ false };

	// Error correction - BER
	vector<t_integer> errCorrBerIn{};
	bool errCorrBerStarted{ false };

	// Privacy amplification seeds
	vector<t_integer> privacySeedsIn{};
	bool privacySeedsStarted{ false };

	bool outputReceivedData(vector <t_integer>& dataVector, Signal& outputSignal, bool &started);

};
#endif // !MESSAGE_PROCESSOR_RECEIVER_H_
