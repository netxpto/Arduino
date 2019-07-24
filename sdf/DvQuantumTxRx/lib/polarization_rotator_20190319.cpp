#include "../include/polarization_rotator_20190319.h"

void PolarizationRotator::initialize(void) {

	numberOfInputSignals = (int)inputSignals.size();

	outputSignals[0]->setFirstValueToBeSaved(inputSignals[0]->getFirstValueToBeSaved());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplesPerSymbol(inputSignals[0]->getSamplesPerSymbol());
}

bool PolarizationRotator::runBlock(void) {
	int ready0 = inputSignals[0]->ready();
	int ready1 = inputSignals[1]->ready();

	int ready = min(ready0, ready1);

	if (numberOfInputSignals == 3) 
		ready = min(ready, inputSignals[2]->ready());
	
	int space = outputSignals[0]->space();

	int process = min(ready, space);

	if (process <= 0) return false;

	for (auto k = 0; k < process; k++) {

		t_complex_xy valueXY;

		t_complex i (0,1);

		if (numberOfInputSignals == 3)
			inputSignals[2]->bufferGet(&phi);

		inputSignals[0]->bufferGet(&valueXY);
		inputSignals[1]->bufferGet(&tetha);

		t_complex valueX = valueXY.x;
		t_complex valueY = valueXY.y;

		//t_complex valueXout = cos(tetha * PI / 180)*valueX + sin(tetha * PI / 180)*valueY*exp(-i*(phi*PI / 180));
		//t_complex ValueYout = -sin(tetha * PI / 180)*valueX*exp(i* (phi*PI/180)) + cos(tetha * PI / 180)*valueY;
		/*t_complex valueXout = cos(tetha)*valueX + sin(tetha)*valueY;
		t_complex ValueYout = -sin(tetha)*valueX + cos(tetha)*valueY;
		t_complex_xy valueXYout = { valueXout, ValueYout };*/

		t_complex valueX_conj = conj(valueX);
		t_complex valueY_conj = conj(valueY);
		/*Jones To Stokes*/
		t_real S0 = real(valueX*valueX_conj + valueY * valueY_conj);
		t_real S1 = real(valueX*valueX_conj - valueY * valueY_conj);
		t_real S2 = real(valueX*valueY_conj + valueY * valueX_conj);
		t_real S3 = real(1i*(valueX*valueY_conj - valueX_conj * valueY));

		t_complex valueXOut{ 0.0 };
		t_complex valueYOut{ 0.0 };

		if (S0 == 0) {
			valueXOut = valueX;
			valueYOut = valueY;
		}
		else {
			t_real S1_out = S1 * cos(phi * 2)*cos(tetha*2) + S2 * (cos(phi * 2)*sin(tetha * 2) + pow(sin(phi * 2), 2)*cos(tetha * 2)) + S3 * (sin(phi * 2)*sin(tetha * 2) - sin(phi * 2)*cos(phi * 2)*cos(tetha * 2));
			t_real S2_out = S1 * (-cos(phi * 2)*sin(tetha * 2)) + S2 * (cos(phi * 2)*cos(tetha * 2) - pow(sin(phi * 2), 2)*sin(tetha * 2)) + S3 * (sin(phi * 2)*cos(tetha * 2) + cos(phi * 2)*sin(phi * 2)*sin(tetha * 2));
			t_real S3_out = S1 * sin(phi * 2) + S2 * (-sin(phi * 2)*cos(phi * 2)) + S3 * pow(cos(phi * 2), 2);

			/*Stokes To jones*/
			//Degree of polarization
			double p = sqrt((double)(pow(S1_out, 2) + pow(S2_out, 2) + pow(S3_out, 2))) / S0;
			/*Normalize the Stokes parameters(first one will be 1, of course)*/
			double Q = S1_out / (S0*p);
			double U = S2_out / (S0*p);
			double V = S3_out / (S0*p);
			/*And construct the 2 Jones components*/
			double A = sqrt((1 + Q) / 2);
			t_complex B{};
			if (A == 0)
				B = 1;
			else
				B = U / (2 * A) - 1i*V / (2 * A);

			valueXOut = (t_complex)A* sqrt(S0*p);
			valueYOut = (t_complex)B* sqrt(S0*p);
		}
		
		t_complex_xy valueXYout = { valueXOut,valueYOut };
		outputSignals[0]->bufferPut((t_complex_xy)valueXYout);

	}

	return true;
}

