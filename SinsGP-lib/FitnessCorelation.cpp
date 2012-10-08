#include "stdafx.h"
#include "FitnessCorelation.h"
#include <cmath>

using namespace SinsGP;
using namespace Beagle;
using namespace std;

FitnessCorelation::FitnessCorelation(unsigned outputs)
    : SinsGP::Fitness<double>(outputs),
      mSignals(outputs),
      mSign(outputs, 0),
      mCorXTime(outputs, 0),
      mCorelation(0),
      mTime(0)
{
}

FitnessCorelation::FitnessCorelation()
    : mCorelation(0),
      mTime(0)
{
}

void FitnessCorelation::postSetData() {
    Beagle_StackTraceBeginM();
    for(unsigned i=0; i<this->size(); ++i) {
        mSignals[i].reserve(mData->getRows());
    }
    Beagle_StackTraceEndM("void SinsGP::FitnessCorelation::postSetData()");
}

void FitnessCorelation::add(const vector<double>& output) {
    Beagle_StackTraceBeginM();
    for(unsigned i=0; i<this->size(); ++i) {
        mSignals[i].push_back(output[i]);
    }
    Beagle_StackTraceEndM("void SinsGP::FitnessCorelation::add()");
}

void FitnessCorelation::commitData() {
    Beagle_StackTraceBeginM();
    mTime += mData->getRows();
    for(unsigned i=0; i<this->size(); ++i) {
        double cor = mData->getOutputs(i).corelation(mSignals[i]);
        // je¿eli któryœ sygna³ jest to¿samoœciowo równy 0, korelacja
        // da wynik NaN. Przyjmujemy wówczas korelacjê równ¹ 0.
        if(isFinite(cor)) {
            mCorXTime[i] += mData->getRows() * cor;
        }
    }
    mSignals = vector<Signal>(this->size());
    Beagle_StackTraceEndM("void SinsGP::FitnessCorelation::commitData()");
}

void FitnessCorelation::evaluate() {
    Beagle_StackTraceBeginM();
    double cor = 0;
    for(unsigned i=0; i<this->size(); ++i) {
        (*this)[i] = abs(mCorXTime[i] / mTime);
        cor += (*this)[i];
        mSign[i] = mCorXTime[i] < 0 ? -1 : 1;
    }
    mCorelation = cor / this->size();
    Beagle_StackTraceEndM("void SinsGP::FitnessCorelation::evaluate()");
}

bool FitnessCorelation::isEqual(const Object &inRightObj) const {
    Beagle_StackTraceBeginM();
    const FitnessCorelation& rightFitness = castObjectT<const FitnessCorelation&>(inRightObj);
    if(isValid() != rightFitness.isValid()) return false;
    if(isValid()==false) return true;
    return mCorelation == rightFitness.mCorelation;
    Beagle_StackTraceEndM("bool SinsGP::FitnessCorelation::isEqual(const Object& inRightObj) const");
}

bool FitnessCorelation::isLess(const Object &inRightObj) const {
    Beagle_StackTraceBeginM();
    const FitnessCorelation& rightFitness = castObjectT<const FitnessCorelation&>(inRightObj);
    if(isValid()==false) {
        return true;
    }
    if(!rightFitness.isValid()) {
        return false;
    }
    return mCorelation < rightFitness.mCorelation;
    Beagle_StackTraceEndM("bool SinsGP::FitnessCorelation::isLess(const Object& inRightObj) const");
}

void FitnessCorelation::write(PACC::XML::Streamer& ioStreamer, bool inIndent) const
{
    Beagle_StackTraceBeginM();
    ioStreamer.openTag("FitnessCorelation", inIndent);
    ioStreamer.insertAttribute("Corelation", dbl2str(mCorelation));
    for(unsigned i=0; i<size(); ++i) {
        ioStreamer.openTag("y" + int2str(i), inIndent);
        ioStreamer.insertAttribute("Corelation", dbl2str((*this)[i]));
        ioStreamer.closeTag();
    }
    ioStreamer.closeTag();
    Beagle_StackTraceEndM("void FitnessCorelation::write(PACC::XML::Streamer& ioStreamer, bool inIndent) const");
}
