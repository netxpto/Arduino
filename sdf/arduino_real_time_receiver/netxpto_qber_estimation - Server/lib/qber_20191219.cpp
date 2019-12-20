# include "../include/qber_20191219.h"
void Qber::initialize(void) {

	for (auto k : outputSignals) {
		k->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
		k->setSamplingPeriod(inputSignals[0]->getSamplingPeriod() / n);
		k->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	}

	if (window > 0) {
		for (auto i = 0; i < window; i++)
			windowBuffer.push_back(0);
	}
}

bool Qber::runBlock(void) {
	/* Computing z */ // This code converges in below 10 steps, exactness chosen in order to achieve this rapid convergence
	limit++;
	if (firstPass)
	{
		firstPass = 0;
		double x1 = -2;
		double x2 = 2;
		double x3 = x2 - (erf(x2 / sqrt(2)) + 1 - alpha) * (x2 - x1) / (erf(x2 / sqrt(2)) - erf(x1 / sqrt(2)));
		double exacteness = 1e-15;

		while (abs(erf(x3 / sqrt(2)) + 1 - alpha) > exacteness)
		{
			x3 = x2 - (erf(x2 / sqrt(2)) + 1 - alpha) * (x2 - x1) / (erf(x2 / sqrt(2)) - erf(x1 / sqrt(2)));
			x1 = x2;
			x2 = x3;
		}

		z = -x3;

	}

	int ready0 = inputSignals[0]->ready();
	int ready1 = inputSignals[1]->ready();
	int ready = min(ready0, ready1);

	int space = outputSignals[0]->space();

	int process = min(ready, space);

	if ((process == 0) && (window < 0)) // Alteraçao do operador && para || to have a stop condition
	{
		QBER = (receivedBits - coincidences) / receivedBits;
		/* Calculating bounds */
		double UpperBound = QBER + 1 / sqrt(receivedBits) * z * sqrt(QBER * (1 - QBER)) + 1 / (3 * receivedBits) * (2 * z * z * (1 / 2 - QBER) + (2 - QBER));
		double LowerBound = QBER - 1 / sqrt(receivedBits) * z * sqrt(QBER * (1 - QBER)) + 1 / (3 * receivedBits) * (2 * z * z * (1 / 2 - QBER) - (1 + QBER));

		/* Outputting a .txt report*/
		ofstream myfile;
		myfile.open(outputSignals[0]->getFolderName() + "/QBER_RAW.txt");
		myfile << "QBER = " << QBER * 100 << " %\n";
		myfile << "Upper and lower confidence bounds for " << (1 - alpha) * 100 << "% confidence level \n";
		myfile << "Upper Bound = " << UpperBound * 100 << " %\n";
		myfile << "Lower Bound = " << LowerBound * 100 << " %\n";
		myfile << "Number of errors = " << receivedBits - coincidences << "\n";
		myfile << "Number of received bits = " << receivedBits << "\n";
		myfile << "No clicks = " << noClicks << "\n";
		myfile << "Double clicks = " << doubleClicks << "\n";
		myfile.close();
		return false;
	}


	for (auto k = 0; k < process; k++) { // process all demands contained in the buffer
		t_binary dataBob{ 0 };
		inputSignals[1]->bufferGet(&dataBob); // data from CoincidenceDetector_

		if (syncMode != sync_mode::Off && nSync)
		{
			t_binary dataAlice{ 0 };
			inputSignals[0]->bufferGet(&dataAlice);
			if (discard)
			{
				discard = false;
				switch (syncMode)
				{
				case sync_mode::On_BobAhead:
					inputSignals[1]->bufferGet(&dataBob);
					ready1--;
					process = min(ready1, space);
					break;
				case sync_mode::On_AliceAhead:
					inputSignals[0]->bufferGet(&dataAlice);
					ready0--;
					process = min(ready0, space);
					break;
				default:
					cout << "QBER syncing routine: No direction given to sync!\n";
					break;
				}

			}

			if (dataBob == 0 | dataBob == 1)
			{
				inBufferSync++;
				if (dataBob == dataAlice) {
					outputSignals[0]->bufferPut((t_binary)2);
					coincidencesSync++;
				}
			}
			if (inBufferSync == bufferSize)
			{
				inBufferSync = 0.0;
				QBER = (1 - coincidencesSync / bufferSize) * 100;
				if (QBER > 100 - berLimit) {
					cout << "Cables are crossed yo!\n";
				}
				if (QBER < berLimit) {
					nSync = false; // if the QBER is bellow the limit, the system is synchronized
					cout << "Synchronization established! \n";
				}
				else discard = true;

				coincidencesSync = 0.0;
			}
		}
		else
		{
			if (window > 0) {  // window represents the number of bits used to calculate the QBER
				if (dataBob == 5.0) { // Control bit
					outputSignals[0]->bufferPut((t_binary)dataBob);
					cBit++;

				}
				else {
					if (cBit > 1) { // If it is not a control bit and there were previously registred control bits
						for (auto k = 0; k < windowBuffer.size(); k++) {
							windowBuffer[k] = 0;
						}
					}
					cBit = 0; // resets control bits

					t_binary dataAlice{ 0 };
					inputSignals[0]->bufferGet(&dataAlice);

					bool fillArray{ false };

					if (dataBob == dataAlice) {
						t_binary out = 0;
						outputSignals[0]->bufferPut((t_binary)out);
						windowBuffer[inBuffer] = 0;
						fillArray = true;
					}
					else if (dataBob == 3.0) { // 00
						t_binary out = 1;
						outputSignals[0]->bufferPut((t_binary)out);
						noClicks++;
					}
					else if (dataBob == 2.0) { // 11
						t_binary out = 1;
						outputSignals[0]->bufferPut((t_binary)out);
						doubleClicks++;
					}
					else {
						t_binary out = 1;
						outputSignals[0]->bufferPut((t_binary)out);

						windowBuffer[inBuffer] = 1;
						fillArray = true;
					}

					if (fillArray) {
						emptyArray = (emptyArray == true) ? false : emptyArray; // if empty array true it becomes false and if it is false it mantains false
						inBuffer = (inBuffer + 1) % lengthWindow;

						double sum{ 0.0 };

						window = lengthWindow;
						if (firstFull == true) {
							window = inBuffer;
							if (inBuffer == (lengthWindow - 1))
								firstFull = false;
						}

						for (auto i = 0; i < window; i++)
							sum = sum + windowBuffer[i];
						coincidences = window - sum;
						QBER = sum / window;
						fillArray = false;
					}

				}
				totalReceivedBits++;
			}

			else { // How can window < 0 ???
				if (dataBob == 5.0) {
					outputSignals[0]->bufferPut((t_binary)dataBob);
				}
				else {
					t_binary dataAlice{ 0 };
					inputSignals[0]->bufferGet(&dataAlice);

					t_binary out_value{ 0 };
					if (dataBob == 3)
						noClicks++;
					else if (dataBob == 2)
						doubleClicks++;
					else {
						if (dataBob == (double)dataAlice) {
							coincidences++;
							out_value = 1;
						}
						receivedBits++;
					}
					totalReceivedBits++;
					outputSignals[0]->bufferPut((t_binary)out_value);
				}
				if (k == (process - 1))
				{
					QBER = (receivedBits - coincidences) / receivedBits;

					/* Calculating bounds */
					double UpperBound = QBER + 1 / sqrt(receivedBits) * z * sqrt(QBER * (1 - QBER)) + 1 / (3 * receivedBits) * (2 * z * z * (1 / 2 - QBER) + (2 - QBER));
					double LowerBound = QBER - 1 / sqrt(receivedBits) * z * sqrt(QBER * (1 - QBER)) + 1 / (3 * receivedBits) * (2 * z * z * (1 / 2 - QBER) - (1 + QBER));

					/* Outputting a .txt report*/
					ofstream myfile;
					myfile.open(outputSignals[0]->getFolderName() + "/QBER_RAW.txt");
					myfile << "QBER = " << QBER * 100 << " %\n";
					myfile << "Upper and lower confidence bounds for " << (1 - alpha) * 100 << "% confidence level \n";
					myfile << "Upper Bound = " << UpperBound * 100 << " %\n";
					myfile << "Lower Bound = " << LowerBound * 100 << " %\n";
					myfile << "Number of errors = " << receivedBits - coincidences << "\n";
					myfile << "Number of received bits to estimate QBER = " << receivedBits << "\n";
					myfile << "No clicks = " << noClicks << "\n";
					myfile << "Double clicks = " << doubleClicks << "\n";
					myfile << "Total Received Bits = " << totalReceivedBits << "\n";
					myfile.close();
				}
			}

			/* Outputting mid reports */
			if (m > 0)
			{
				if ((remainder(totalReceivedBits, m) == 0) & (emptyArray == false))
				{
					if ((QBER * 100 > berLimit) && syncMode != sync_mode::Off) {
						nSync = true; // bye bye bye
						cout << "Synchronization lost, attempting to recover! \n";
						inBuffer = 0;
					}

					string filename;
					ostringstream oss;
					if (nSync && syncMode != sync_mode::Off) {
						numberSyncLossReport++;
						oss << "midreportSyncLoss" << numberSyncLossReport << ".txt";
						filename = oss.str();
					}
					else
					{
						n++;
						oss << "midreport" << n << ".txt";
						filename = oss.str();
					}
					/* Calculating bounds */
					double UpperBound = QBER + 1 / sqrt(window) * z * sqrt(QBER * (1 - QBER)) + 1 / (3 * window) * (2 * z * z * (1 / 2 - QBER) + (2 - QBER));
					double LowerBound = QBER - 1 / sqrt(window) * z * sqrt(QBER * (1 - QBER)) + 1 / (3 * window) * (2 * z * z * (1 / 2 - QBER) - (1 + QBER));

					/* Outputting a .txt report*/
					ofstream myfile;
					myfile.open(filename);
					if (nSync) myfile << "### SYNCHRONIZATION LOST! ###\n";
					myfile << "QBER = " << QBER * 100 << " %\n";
					myfile << "Upper and lower confidence bounds for " << (1 - alpha) * 100 << "% confidence level \n";
					myfile << "Upper Bound = " << UpperBound * 100 << " %\n";
					myfile << "Lower Bound = " << LowerBound * 100 << " %\n";
					myfile << "Number of errors = " << window - coincidences << "\n";
					myfile << "Number of received bits to estimate QBER = " << window << "\n";
					myfile << "No clicks = " << noClicks << "\n";
					myfile << "Double clicks = " << doubleClicks << "\n";
					myfile << "Total Received Bits = " << totalReceivedBits << "\n";
					myfile.close();



				}
			}
		}
	}
	return true;
}