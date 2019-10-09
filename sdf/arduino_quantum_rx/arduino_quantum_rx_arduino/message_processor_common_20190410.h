#ifndef MESSAGE_PROCESSOR_COMMON_H_
#define MESSAGE_PROCESSOR_COMMON_H_

#include <vector>
#include "netxpto_20180815.h"

typedef enum { BasisReconciliationMsg, ErrorCorrectionMsg, ErrorCorrectionPermutationsMsg, PrivacyAmplificationSeedsMsg, ParameterEstimationSeedAndBitsMsg, ToggleBERChangeMsg} t_message_type;
typedef int t_message_data_length;
typedef vector<int> t_message_data;

namespace MessageProcessors {
	t_message_data getMessageData(const t_message& msg, t_message_data_length dataLength);
	t_message_data_length getMessageDataLength(const t_message& msg);
	t_message_type getMessageType(const t_message& msg);

}


#endif // MESSAGE_PROCESSOR_COMMON_H_