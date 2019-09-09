# include "..\include\binary_source_20180815.h"
# include "..\include\coincidence_detector_20190319.h"
# include "..\include\netxpto_20180815.h"
# include "..\include\qber_20190418.h"
# include "..\include\sink_20180815.h"

string s = "0";
string s1 = "1";


int main()
{
	/* Signals Declaration */
	Binary SPD0_out{ "SPD0_out.sgn"};
	SPD0_out.setSaveInAscii(true);

	Binary SPD1_out{ "SPD1_out.sgn"};
	SPD1_out.setSaveInAscii(true);

	TimeDiscreteAmplitudeDiscreteReal CD_out{ "CD_out.sgn"};
	CD_out.setSaveInAscii(true);

	Binary Alice_out{ "Alice_out.sgn"};
	Alice_out.setSaveInAscii(true);

	Binary QBER_out{ "QBER_out.sgn"};
	QBER_out.setSaveInAscii(true);

	/* Blocks Decalration */
	BinarySource BinarySource0_{ {},{ &SPD0_out} };
	BinarySource0_.setBitStream(s);
	BinarySource0_.setNumberOfBits(10000);
	BinarySource0_.setMode(BinarySourceMode::DeterministicCyclic);


	BinarySource BinarySource1_{ {},{ &SPD1_out} };
	BinarySource1_.setBitStream(s1);
	BinarySource1_.setNumberOfBits(10000);
	BinarySource1_.setMode(BinarySourceMode::DeterministicCyclic);


	BinarySource BinarySource2_{ {},{ &Alice_out} };
	BinarySource2_.setBitStream(s1);
	BinarySource2_.setNumberOfBits(10000);
	BinarySource2_.setMode(BinarySourceMode::DeterministicCyclic);
	CoincidenceDetector CoincidenceDetector_{ {&SPD0_out, &SPD1_out},{&CD_out} };

	Qber QBER_{ {&Alice_out, &CD_out}, {&QBER_out} };
	

	Sink Sink_QBER_{ { &QBER_out },{} };
	Sink_QBER_.setDisplayNumberOfSamples(true);
	


	System MainSystem{
		// BLOCKS
		&BinarySource0_,
		&BinarySource1_,
		&CoincidenceDetector_,
		&BinarySource2_,
		&QBER_,
		&Sink_QBER_


	};

	MainSystem.run();
	MainSystem.terminate();
	

	system("pause");

	return 0;
}
