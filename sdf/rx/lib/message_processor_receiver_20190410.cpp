#include "..\include\message_processor_receiver_20190410.h"

void MessageProcessorReceiver::initialize(void) {}

bool MessageProcessorReceiver::runBlock(void)
{
	bool alive = false;

	int ready = inputSignals[0]->ready();
	// Ready is the number of messages in buffer. Not directly related to outputSignals space.

	if (0 <= ready) {
		for (auto k = 0; k < ready; ++k) {
			t_message messageIn;
			inputSignals[0]->bufferGet(&messageIn);

			t_message_type mType = MessageProcessors::getMessageType(messageIn);
			t_message_data_length mDataLength = MessageProcessors::getMessageDataLength(messageIn);
			t_message_data mData;

			if (t_message_type::BasisReconciliationMsg == mType) {
				basisIn = MessageProcessors::getMessageData(messageIn, mDataLength);
				alive = true;
			}
			else if (t_message_type::ErrorCorrectionMsg == mType) {
				parityIn = MessageProcessors::getMessageData(messageIn, mDataLength);
				alive = true;
			}
			else if (t_message_type::ErrorCorrectionPermutationsMsg == mType) {
				permutationsIn = MessageProcessors::getMessageData(messageIn, mDataLength);
				alive = true;
			}
			else if (t_message_type::PrivacyAmplificationSeedsMsg == mType) {
				privacySeedsIn = MessageProcessors::getMessageData(messageIn, mDataLength);
				alive = true;
			}
			else if (t_message_type::ParameterEstimationSeedAndBitsMsg == mType) {
				paramEstIn = MessageProcessors::getMessageData(messageIn, mDataLength);
				alive = true;
			}
			else if (t_message_type::ToggleBERChangeMsg == mType) {
				errCorrBerIn = MessageProcessors::getMessageData(messageIn, mDataLength);
				alive = true;
			}
		}
	}

	alive = outputReceivedData(basisIn, *outputSignals[0], basisReconRcvStarted) || alive;
	if (outputSignals.size() >= 2) alive = outputReceivedData(parityIn, *outputSignals[1], errCorrParitiesStarted) || alive;
	if (outputSignals.size() >= 3) alive = outputReceivedData(permutationsIn, *outputSignals[2], errCorrPermStarted) || alive;
	if (outputSignals.size() >= 4) alive = outputReceivedData(privacySeedsIn, *outputSignals[3], privacySeedsStarted) || alive;
	if (outputSignals.size() >= 5) alive = outputReceivedData(paramEstIn, *outputSignals[4], privacySeedsStarted) || alive;
	if (outputSignals.size() >= 6) alive = outputReceivedData(errCorrBerIn, *outputSignals[5], paramEstRcvStarted) || alive;

	/*// Output Parameter Estimation
	int spacePE = outputSignals[4]->space();
	if ((paramEstIn.size() > 0) && (spacePE > 0)) {

		if (!paramEstRcvStarted) {
			outputSignals[4]->bufferPut((t_real)paramEstIn.size());
			paramEstRcvStarted = true;
			spacePE--;
		}

		int process = min(spacePE, (int)paramEstIn.size());

		for (auto m = 0; m < process; ++m) {
			outputSignals[4]->bufferPut((t_real)paramEstIn[m]);
		}
		paramEstIn.erase(paramEstIn.begin(), paramEstIn.begin() + process);
		if (0 == paramEstIn.size()) paramEstRcvStarted = false;
		alive = true;
	}*/

	return alive;
}

bool MessageProcessorReceiver::outputReceivedData(vector<t_integer>& dataVector, Signal& outputSignal, bool& started)
{
	bool alive = false;
	int space = outputSignal.space();

	if ((dataVector.size() > 0) && (space > 0)) {

		if (!started) {
			outputSignal.bufferPut((t_real)dataVector.size());
			started = true;
			space--;
		}

		int process = min(space, (int)dataVector.size());

		for (auto m = 0; m < process; ++m) {
			outputSignal.bufferPut((t_real)dataVector[m]);
		}
		dataVector.erase(dataVector.begin(), dataVector.begin() + process);
		if (0 == dataVector.size()) started = false;
		alive = true;

	}

	return alive;
}