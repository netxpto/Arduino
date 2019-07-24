# include "../include/qber_20190418.h"

void Qber::initialize(void) {

	for (auto k : outputSignals) {
		k->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
		k->setSamplingPeriod(inputSignals[0]->getSamplingPeriod()/n);
		k->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	}

	if (window > 0) {
		for (auto i = 0; i < window; i++)
			windowBuffer.push_back(0);
	}
}

bool Qber::runBlock(void) {
	/* Computing z */ // This code converges in below 10 steps, exactness chosen in order to achieve this rapid convergence
	if (firstPass)
	{
		firstPass = 0;
		double x1 = -2;
		double x2 = 2;
		double x3 = x2 - (erf(x2 / sqrt(2)) + 1 - alpha)*(x2 - x1) / (erf(x2 / sqrt(2)) - erf(x1 / sqrt(2)));
		double exacteness = 1e-15;

		while (abs(erf(x3 / sqrt(2)) + 1 - alpha) > exacteness)
		{
			x3 = x2 - (erf(x2 / sqrt(2)) + 1 - alpha)*(x2 - x1) / (erf(x2 / sqrt(2)) - erf(x1 / sqrt(2)));
			x1 = x2;
			x2 = x3;
		}

		z = -x3;

	}

	int ready{ MAX_BUFFER_LENGTH };
	for (auto k : inputSignals) {
		ready = min(k->ready(), ready);
	}

	int space = { MAX_BUFFER_LENGTH };
	for (auto k : outputSignals) {
		space = min(k->space(), space);
	}

	int process = min(ready, space);

	if ((process == 0) && (window < 0))
	{
		QBER = (receivedBits - coincidences) / receivedBits;
		/* Calculating bounds */
		double UpperBound = QBER + 1 / sqrt(receivedBits) * z  * sqrt(QBER*(1 - QBER)) + 1 / (3 * receivedBits)*(2 * z * z * (1 / 2 - QBER) + (2 - QBER));
		double LowerBound = QBER - 1 / sqrt(receivedBits) * z  * sqrt(QBER*(1 - QBER)) + 1 / (3 * receivedBits)*(2 * z * z * (1 / 2 - QBER) - (1 + QBER));

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

	for (auto k = 0; k < process; k++) {
		t_real dataBob{ 0.0 };
		inputSignals[1]->bufferGet(&dataBob);

		if (window > 0) {
			if (dataBob == 5.0) {
				outputSignals[0]->bufferPut((t_binary)dataBob);
				cBit++;
				
			}
			else {
				if (cBit > 1) {
					for (auto k = 0; k < windowBuffer.size(); k++) {
						windowBuffer[k] = 0;
					}
				}
				cBit = 0;

				t_binary dataAlice{ 0 };
				inputSignals[0]->bufferGet(&dataAlice);

				bool fillArray{ false };

				if (dataBob == dataAlice) {
					windowBuffer[inBuffer] = 0;
					fillArray = true;
				}
				else if (dataBob == 3.0) {
					noClicks++;
				}
				else if (dataBob == 2.0) {
					doubleClicks++;
				}
				else {
					windowBuffer[inBuffer] = 1;
					fillArray = true;
				}
					
				if (fillArray) {
					emptyArray = (emptyArray == true) ? false : emptyArray;
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
		
		else {
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
				double UpperBound = QBER + 1 / sqrt(receivedBits) * z  * sqrt(QBER*(1 - QBER)) + 1 / (3 * receivedBits)*(2 * z * z * (1 / 2 - QBER) + (2 - QBER));
				double LowerBound = QBER - 1 / sqrt(receivedBits) * z  * sqrt(QBER*(1 - QBER)) + 1 / (3 * receivedBits)*(2 * z * z * (1 / 2 - QBER) - (1 + QBER));

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
				n++;

				ostringstream oss;
				oss << "midreport" << n << ".txt";
				string filename = oss.str();

				/* Calculating bounds */
				double UpperBound = QBER + 1 / sqrt(window) * z  * sqrt(QBER*(1 - QBER)) + 1 / (3 * window)*(2 * z * z * (1 / 2 - QBER) + (2 - QBER));
				double LowerBound = QBER - 1 / sqrt(window) * z  * sqrt(QBER*(1 - QBER)) + 1 / (3 * window)*(2 * z * z * (1 / 2 - QBER) - (1 + QBER));

				/* Outputting a .txt report*/
				ofstream myfile;
				myfile.open(filename);
	
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

	return true;
}