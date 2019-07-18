#include "..\include\message_processor_transmitter_20190410.h"

void MessageProcessorTransmitter::initialize(void) {}

bool MessageProcessorTransmitter::runBlock(void)
{
	bool alive = false;

	int ready0 = inputSignals[0]->ready();
	int ready1 = inputSignals[1]->ready();
	int ready2{ 0 };
	int ready3{ 0 };
	int ready4{ 0 };
	int ready5{ 0 };
	int ready;
	ready = max(ready0, ready1);

	if (inputSignals.size() >= 3) {
		ready2 = inputSignals[2]->ready();
		ready = max(ready, ready2);

	}
	if (inputSignals.size() >= 4) {
		ready3 = inputSignals[3]->ready();
		ready = max(ready, ready3);
	}
	if (inputSignals.size() >= 5) {
		ready4 = inputSignals[4]->ready();
		ready = max(ready, ready4);
	}
	if (inputSignals.size() >= 6) {
		ready5 = inputSignals[5]->ready();
		ready = max(ready, ready5);
	}

	int space = outputSignals[0]->space();

	if (0 == ready || 0 == space) return alive;


	// Basis reconciliation message
	if ((ready0 > 0) && (outputSignals[0]->space() > 0)) {
		t_integer startIdx{ 0 };
		if (!basisReconStarted) {
			inputSignals[0]->bufferGet(&basisReconLength);
			basisReconStarted = true;
			startIdx = 1;
		}

		t_real basisReconData;
		for (auto k = startIdx; k < ready0; ++k) {
			inputSignals[0]->bufferGet(&basisReconData);
			basisReconVector.push_back((t_integer)basisReconData);
		}

		if (basisReconVector.size() == basisReconLength) {
			t_message_type msgType = BasisReconciliationMsg;
			string messageText = MessageProcessors::generateMessageData(basisReconVector, msgType);

			t_message messageBasisRecon;
			messageBasisRecon.messageData = messageText;
			messageBasisRecon.messageDataLength = to_string((t_message_data_length)messageText.size());
			messageBasisRecon.messageType = msgType;

			outputSignals[0]->bufferPut(messageBasisRecon);
			basisReconVector.erase(basisReconVector.begin(), basisReconVector.end());
			basisReconStarted = false;

		}
		alive = true;
	}


	// Error correction - Parities 
	if ((ready1 > 0) && (outputSignals[0]->space() > 0)) {
		t_integer startIdx{ 0 };
		if (!ErrCorrParityStarted) {
			inputSignals[1]->bufferGet(&ErrCorrParityLength);
			ErrCorrParityStarted = true;
			startIdx = 1;
		}

		t_real paritiesData;
		for (auto k = startIdx; k < ready1; ++k) {
			inputSignals[1]->bufferGet(&paritiesData);
			ErrCorrParityVector.push_back((t_integer)paritiesData);
		}

		if (ErrCorrParityVector.size() == ErrCorrParityLength) {
			t_message_type msgType = ErrorCorrectionMsg;
			string messageText = MessageProcessors::generateMessageData(ErrCorrParityVector, msgType);

			t_message errCorrParityMsg;
			errCorrParityMsg.messageData = messageText;
			errCorrParityMsg.messageDataLength = to_string((t_message_data_length)messageText.size());
			errCorrParityMsg.messageType = msgType;

			outputSignals[0]->bufferPut(errCorrParityMsg);
			ErrCorrParityVector.erase(ErrCorrParityVector.begin(), ErrCorrParityVector.end());
			ErrCorrParityStarted = false;

		}
		alive = true;
	}

	// Error Correction Permutations
	if ((ready2 > 0) && (outputSignals[0]->space() > 0)) {
		t_integer startIdx{ 0 };
		if (!ErrCorrPermStarted) {
			inputSignals[2]->bufferGet(&ErrCorrPermLength);
			ErrCorrPermStarted = true;
			startIdx = 1;
		}

		t_real permutationsIn;
		for (auto k = startIdx; k < ready2; ++k) {
			inputSignals[2]->bufferGet(&permutationsIn);
			ErrCorrPermVector.push_back((t_integer)permutationsIn);
		}

		if (ErrCorrPermVector.size() == ErrCorrPermLength) {
			t_message_type msgType = ErrorCorrectionPermutationsMsg;
			string messageText = MessageProcessors::generateMessageData(ErrCorrPermVector, msgType);

			t_message messageErrorCorrection;
			messageErrorCorrection.messageData = messageText;
			messageErrorCorrection.messageDataLength = to_string((t_message_data_length)messageText.size());
			messageErrorCorrection.messageType = msgType;

			outputSignals[0]->bufferPut(messageErrorCorrection);
			ErrCorrPermVector.erase(ErrCorrPermVector.begin(), ErrCorrPermVector.end());
			ErrCorrPermStarted = false;

		}
		alive = true;
	}

	// Transmit privacy amplification hashing seeds
	if ((ready3 > 0) && (outputSignals[0]->space() > 0)) {
		t_integer startIdx{ 0 };
		if (!privacySeedsStarted) {
			inputSignals[3]->bufferGet(&privacySeedsLength);
			privacySeedsStarted = true;
			startIdx = 1;
		}

		t_real paramEstData;
		for (auto k = startIdx; k < ready3; ++k) {
			inputSignals[3]->bufferGet(&paramEstData);
			privacySeedsBVector.push_back((t_integer)paramEstData);
		}

		if (privacySeedsBVector.size() == privacySeedsLength) {
			t_message_type msgType = PrivacyAmplificationSeedsMsg;
			string messageText = MessageProcessors::generateMessageData(privacySeedsBVector, msgType);

			t_message messagePASeeds;
			messagePASeeds.messageData = messageText;
			messagePASeeds.messageDataLength = to_string((t_message_data_length)messageText.size());
			messagePASeeds.messageType = msgType;

			outputSignals[0]->bufferPut(messagePASeeds);
			privacySeedsBVector.erase(privacySeedsBVector.begin(), privacySeedsBVector.end());
			privacySeedsStarted = false;

		}
		alive = true;
	}

	//////////////////////////////////////////////////
	// Parameter estimation seed and bits

	if ((ready4 > 0) && (outputSignals[0]->space() > 0)) {
		t_integer startIdx{ 0 };
		t_message_type msgType = ParameterEstimationSeedAndBitsMsg;
		if (!ParamEstStarted) {
			inputSignals[4]->bufferGet(&paramEstLength);
			ParamEstStarted = true;
			startIdx = 1;
		}

		t_real paramEstData;
		for (auto k = startIdx; k < ready4; ++k) {
			inputSignals[4]->bufferGet(&paramEstData);
			paramEstVector.push_back((t_integer)paramEstData);
		}

		if (paramEstVector.size() == paramEstLength) {

			string messageText = MessageProcessors::generateMessageData(paramEstVector, msgType);

			t_message messageParamEst;
			messageParamEst.messageData = messageText;
			messageParamEst.messageDataLength = to_string((t_message_data_length)messageText.size());
			messageParamEst.messageType = msgType;

			outputSignals[0]->bufferPut(messageParamEst);
			paramEstVector.erase(paramEstVector.begin(), paramEstVector.end());
			ParamEstStarted = false;

		}
		alive = true;
	}

	// Change BER value
	if ((ready5 > 0) && (outputSignals[0]->space() > 0)) {
		t_integer startIdx{ 0 };
		if (!ErrCorrBerStarted) {
			inputSignals[5]->bufferGet(&ErrCorrBerLength);
			ErrCorrBerStarted = true;
			startIdx = 1;
		}

		t_real berChangeIn;
		for (auto k = startIdx; k < ready5; ++k) {
			inputSignals[5]->bufferGet(&berChangeIn);
			ErrCorrBerVector.push_back((t_integer)berChangeIn);
		}

		if (ErrCorrBerVector.size() == ErrCorrBerLength) {
			t_message_type msgType = ToggleBERChangeMsg;
			string messageText = MessageProcessors::generateMessageData(ErrCorrBerVector, msgType);

			t_message messageTBC;
			messageTBC.messageData = messageText;
			messageTBC.messageDataLength = to_string((t_message_data_length)messageText.size());
			messageTBC.messageType = msgType;

			outputSignals[0]->bufferPut(messageTBC);
			ErrCorrBerVector.erase(ErrCorrBerVector.begin(), ErrCorrBerVector.end());
			ErrCorrBerStarted = false;
		}
		alive = true;
	}


	return alive;
}
