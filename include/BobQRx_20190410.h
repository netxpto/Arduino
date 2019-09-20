# ifndef BOB_H_
# define BOB_H_

# include "../include/netxpto_20180815.h"

# include "../include/binary_source_20180815.h"
# include "../include/m_qam_mapper_20180815.h"
# include "../include/discrete_to_continuous_time_20180815.h"
# include "../include/sink_20180815.h"
# include "../include/pulse_shaper_20180815.h"
# include "../include/time_delay_20180815.h"
# include "../include/polarization_rotator_20181113.h"
# include "../include/fork_20180815.h"
# include "../include/polarization_beam_splitter_20181113.h"
# include "../include/single_photon_detector_20181113.h"
# include "../include/coincidence_detector_20181113.h"
# include "../include/clock_20181113.h"
# include "../include/random_sop_compensation_20190401.h"
# include "../include/electronic_polarization_controller_20181220.h"

class BobQRx : public SuperBlock {

public:

	//##############################################################################################################

	BobQRx(initializer_list<Signal *> inputSig, initializer_list<Signal *> outputSig);

	void initialize(void);
	bool runBlock(void);

	/*Input Parameters*/
	bool logValue{ true };
	string logFileName{ "SuperBlock_Bob.txt" };
	string signalsFolderName{ "signals/SuperBlock_Bob" };
	bool firstTime{ true };

	//##############################################################################################################
	void setM(int mValue) { MQamMapper_.setM(mValue); };
	void setIqAmplitudes(vector<vector<t_real>> iqAmplitudesValues) { MQamMapper_.setIqAmplitudes(iqAmplitudesValues); };

	void setFirstTime(bool fTime) { MQamMapper_.setFirstTime(fTime); };
	bool getFirstTime() { return MQamMapper_.getFirstTime(); };

	void setNumberOfSamplesPerSymbol(int nSamplesPerSymbol) { DiscreteToContinuousTime_.setNumberOfSamplesPerSymbol(nSamplesPerSymbol); };
	int const getNumberOfSamplesPerSymbol(void) { return DiscreteToContinuousTime_.getNumberOfSamplesPerSymbol(); };
	/*
	void setNumberOfSamplesPerSymbolSPD(int nSamplesPerSymbol) { DiscreteToContinuousTime_1.setNumberOfSamplesPerSymbol(nSamplesPerSymbol); };
	int const getNumberOfSamplesPerSymbolSPD(void) { return DiscreteToContinuousTime_1.getNumberOfSamplesPerSymbol(); };

	void setTimeDelaySPD(double tDelay) { TimeDelay_.setTimeDelay(tDelay); };
	double const getTimeDelaySPD() { TimeDelay_.getTimeDelay(); };
	*/
	void setImpulseResponseTimeLength(int impResponseTimeLength) { PulseShaper_.setImpulseResponseTimeLength_symbolPeriods(impResponseTimeLength); };
	int const getImpulseResponseTimeLength(void) { return PulseShaper_.getImpulseResponseTimeLength_symbolPeriods(); };

	void setFilterType(pulse_shapper_filter_type fType) { PulseShaper_.setFilterType(fType); };
	pulse_shapper_filter_type const getFilterType(void) { return PulseShaper_.getFilterType(); };

	void setRollOffFactor(double rOffFactor) { PulseShaper_.setRollOffFactor(rOffFactor); };
	double const getRollOffFactor() { return PulseShaper_.getRollOffFactor(); };

	void setPulseWidth(double pWidth) { PulseShaper_.setPulseWidth(pWidth); };
	double const getPulseWidth() { return PulseShaper_.getPulseWidth(); };

	void setSPD0Path(int path0) { SinglePhotonDetector_0.setPath(path0); };
	int const getSPD0Path() { return SinglePhotonDetector_0.getPath(); };

	void setSPD1Path(int path1) { SinglePhotonDetector_1.setPath(path1); };
	int const getSPD1Path() { return SinglePhotonDetector_1.getPath(); };

	void setSPDProbabilityDarkCount(double pDarkC) { SinglePhotonDetector_0.setProbabilityDarkCount(pDarkC); SinglePhotonDetector_1.setProbabilityDarkCount(pDarkC); };
	double const getSPD0dc() { return SinglePhotonDetector_0.getProbabilityDarkCount(); };
	double const getSPD1dc() { return SinglePhotonDetector_1.getProbabilityDarkCount(); };

	void setClockPeriod(double cperiod) { Clock_.setClockPeriod(cperiod); };
	double const getClockPeriod() { return Clock_.getClockPeriod(); };

	void setClockPhase(double cphase) { Clock_.setClockPhase(cphase); };
	double const getClockPhase() { Clock_.getClockPhase(); };

	void setClockSamplingPeriod(double csPeriod) { Clock_.setSamplingPeriod(csPeriod); };
	double const getClockSamplingPeriod() { return Clock_.getSamplingPeriod(); };

	void setDecisionLevel(double dLevel) { CoincidenceDetector_.setDecisionLevel(dLevel); };
	double const getDecisionLevel() { return CoincidenceDetector_.getDecisionLevel(); };

	void setFilterType_PSclk(pulse_shapper_filter_type fType) { PulseShaper_cd.setFilterType(fType); };
	pulse_shapper_filter_type const getFilterType_PSclk(void) { return PulseShaper_cd.getFilterType(); };

	void setPulseWidthDetection(double pWidth) { PulseShaper_cd.setPulseWidth(pWidth); };
	double const getPulseWidthDetection() { return PulseShaper_cd.getPulseWidth(); };

	void setControlSequenceBits(string cSeq) { RandomSopCompensation_.setDataSequence(cSeq); };
	string getControlSequenceBits() { return RandomSopCompensation_.getDataSequence(); };


	//###############################################################################################################

private:
	
	Binary BasisQRx{ "BasisQRx.sgn" };
	TimeDiscreteAmplitudeContinuousReal MQM_out{ "MQM_out.sgn" }, MQM_clk{ "MQM_clk.sgn" };
	MQamMapper MQamMapper_{ { &BasisQRx },{ &MQM_out, &MQM_clk } };

	TimeContinuousAmplitudeContinuousReal DTCT_clk{ "DTCT_clk.sgn" };
	DiscreteToContinuousTime DiscreteToContinuousTime_1{ { &MQM_clk },{ &DTCT_clk } };

	TimeContinuousAmplitudeContinuousReal TD_clk{ "TD_clk.sgn" };
	TimeDelay TimeDelay_{ { &DTCT_clk },{ &TD_clk } };

	TimeContinuousAmplitudeContinuousReal PS_clk{ "PS_clk.sgn" };
	PulseShaper PulseShaper_clk{ { &TD_clk },{ &PS_clk } };

	TimeContinuousAmplitudeContinuousReal Trigger_SPD0{ "Trigger_SPD0.sgn" }, Trigger_SPD1{ "Trigger_SPD1.sgn" };
	Fork Fork_{ { &PS_clk },{ &Trigger_SPD0, &Trigger_SPD1} };

	TimeContinuousAmplitudeContinuousReal DTCT_out{ "DTCT_out.sgn" };
	DiscreteToContinuousTime DiscreteToContinuousTime_{ { &MQM_out },{ &DTCT_out } };

	TimeContinuousAmplitudeContinuousReal PS_out{ "PS_out.sgn" };
	PulseShaper PulseShaper_{ { &DTCT_out },{ &PS_out } };

	PhotonStreamXY Bob_in{ "Bob_in.sgn" }, SPPBS_in{ "SPPBS_in.sgn" }, EPC_Out{ "EPC_Out.sgn" };
	ElectronicPolarizationController ElectronicPC_{ { &Bob_in, &Theta1_DTCT, &Theta2_DTCT, &Theta3_DTCT },{ &EPC_Out } };

	PolarizationRotator PolarizationRotator_{ { &EPC_Out, &PS_out },{ &SPPBS_in } };

	PhotonStreamMPXY SPPBS_out{ "SPPBS_out.sgn" };
	PolarizationBeamSplitter PolarizationBeamSplitter_{ { &SPPBS_in },{ &SPPBS_out } };

	TimeContinuousAmplitudeContinuousReal SPD0_out{ "SPD0_out.sgn" }, SPD1_out{ "SPD1_out.sgn" };
	SinglePhotonDetector SinglePhotonDetector_0{ { &SPPBS_out, &Trigger_SPD0 },{ &SPD0_out } };
	SinglePhotonDetector SinglePhotonDetector_1{ { &SPPBS_out, &Trigger_SPD1 },{ &SPD1_out } };

	TimeContinuousAmplitudeContinuousReal CLK_cd_in{ "CLK_cd.sgn" };
	Clock Clock_{ {},{ &CLK_cd_in } };

	TimeContinuousAmplitudeContinuousReal TD_clk_1{ "TD_clk_cd.sgn" };
	TimeDelay TimeDelay_clk{ { &CLK_cd_in },{ &TD_clk_1 } };

	TimeContinuousAmplitudeContinuousReal PS_clk_cd{ "PS_clk_cd.sgn" };
	PulseShaper PulseShaper_cd{ { &TD_clk_1 },{ &PS_clk_cd } };

	TimeDiscreteAmplitudeContinuousReal CD_out{ "CD_out.sgn" }, CD2_out{ "CD2_out.sgn" };;
	CoincidenceDetector CoincidenceDetector_{ { &SPD0_out, &SPD1_out, &PS_clk_cd },{ &CD_out , &CD2_out} };

	TimeDiscreteAmplitudeContinuousReal  RSP_theta1_out{ "RSP_theta1_out.sgn", 1 }, RSP_theta2_out{ "RSP_theta2_out.sgn", 1 }, RSP_theta3_out{ "RSP_theta3_out.sgn", 1 };
	Binary ModeSelection{ "ModeSelection.sgn" };
	RandomSopCompensation RandomSopCompensation_{ { &CD_out },{ &RSP_theta1_out, &RSP_theta2_out, &RSP_theta3_out, &ModeSelection } };

	TimeContinuousAmplitudeContinuousReal Theta1_DTCT{ "Theta1_DTCT.sgn", 16 }, Theta2_DTCT{ "Theta2_DTCT.sgn", 16 }, Theta3_DTCT{ "Theta3_DTCT.sgn", 16 };
	DiscreteToContinuousTime DiscreteToContinuousTheta1{ { &RSP_theta1_out },{ &Theta1_DTCT } };
	DiscreteToContinuousTime DiscreteToContinuousTheta2{ { &RSP_theta2_out },{ &Theta2_DTCT } };
	DiscreteToContinuousTime DiscreteToContinuousTheta3{ { &RSP_theta3_out },{ &Theta3_DTCT } };

};

# endif


