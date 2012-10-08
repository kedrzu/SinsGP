#include "stdafx.h"
#include "FitnessNMSE.h"
#include <limits>

using namespace SinsGP;
using namespace Beagle;

FitnessNMSE::FitnessNMSE(unsigned outputs)
    : SinsGP::Fitness<NMSE>(outputs),
      mNMSE(0),
      mPow2SumBuffer(outputs, 0),
      mPow2Sum(outputs, 0),
      mPow2SumNorm(outputs, 0),
      mStep(0)
{
}

void FitnessNMSE::add(std::vector<double> output) {
    Beagle_StackTraceBeginM();
    for(unsigned i=0; i<this->size(); ++i) {
        double error = mData->getOutputs(i)[mStep] - output[i];
        mPow2SumBuffer[i] += error*error;
    }
    mStep++;
    Beagle_StackTraceEndM("void SinsGP::FitnessNMSE::add()");
}

void FitnessNMSE::commitData() {
    Beagle_StackTraceBeginM();
    for(unsigned i=0; i<this->size(); ++i) {
        mPow2Sum[i] += mPow2SumBuffer[i];
        mPow2SumNorm[i] += mPow2SumBuffer[i] / mData->getOutputs(i).getStats().rms;
    }
    mPow2SumBuffer = std::vector<double>(this->size(), 0);
    mStep = 0;
    mTime += mData->getRows();
    Beagle_StackTraceEndM("void SinsGP::FitnessNMSE::commitData()");
}

void FitnessNMSE::evaluate() {
    Beagle_StackTraceBeginM();
    double err = 0;
    for(unsigned i=0; i<this->size(); ++i) {
        (*this)[i].mse = mPow2Sum[i] / mTime;
        (*this)[i].nmse = mPow2SumNorm[i] / mTime;
        err += (*this)[i].nmse;
    }
    mNMSE = err / this->size();
    Beagle_StackTraceEndM("void SinsGP::FitnessNMSE::evaluate()");
}

bool FitnessNMSE::isEqual(const Object &inRightObj) const {
    Beagle_StackTraceBeginM();
    const FitnessNMSE& rightFitness = castObjectT<const FitnessNMSE&>(inRightObj);
    if(isValid() != rightFitness.isValid()) return false;
    if(isValid()==false) return true;
    return mNMSE == rightFitness.mNMSE;
    Beagle_StackTraceEndM("bool SinsGP::FitnessNMSE::isEqual(const Object& inRightObj) const");
}

bool FitnessNMSE::isLess(const Object &inRightObj) const {
    Beagle_StackTraceBeginM();
    const FitnessNMSE& rightFitness = castObjectT<const FitnessNMSE&>(inRightObj);
    if(isValid()==false) {
        return true;
    }
    if(!rightFitness.isValid()) {
        return false;
    }
    return mNMSE > rightFitness.mNMSE;
    Beagle_StackTraceEndM("bool SinsGP::FitnessNMSE::isLess(const Object& inRightObj) const");
}

void FitnessNMSE::write(PACC::XML::Streamer& ioStreamer, bool inIndent) const
{
    Beagle_StackTraceBeginM();
    ioStreamer.openTag("FitnessNMSE", inIndent);
    ioStreamer.insertAttribute("NMSE", dbl2str(mNMSE));
    for(unsigned i=0; i<size(); ++i) {
        ioStreamer.openTag("y" + int2str(i), inIndent);
        ioStreamer.insertAttribute("NMSE", dbl2str((*this)[i].nmse));
        ioStreamer.insertAttribute("MSE", dbl2str((*this)[i].mse));
        ioStreamer.closeTag();
    }
    ioStreamer.closeTag();
    Beagle_StackTraceEndM("void FitnessSimple::write(PACC::XML::Streamer& ioStreamer, bool inIndent) const");
}
