#include "stdafx.h"
#include "FitnessNMSE.h"
#include <limits>

using namespace SinsGP;
using namespace Beagle;

FitnessNMSE::FitnessNMSE(unsigned outputs)
    : Beagle::Fitness(false), std::vector<NMSE>(outputs), mNMSE(0)
{
}

FitnessNMSE::FitnessNMSE()
    : Beagle::Fitness(false), mNMSE(0)
{
}

void FitnessNMSE::evaluate() {
    Beagle_StackTraceBeginM();
    double err = 0;
    for(unsigned i=0; i<this->size(); ++i) {
        err += (*this)[i].nmse;
    }
    mNMSE = err / this->size();
    setValid();
    Beagle_StackTraceEndM("bool SinsGP::void FitnessNMSE::evaluate()");
}

FitnessNMSE::Handle FitnessNMSE::unstable(unsigned outputs) {
    Beagle_StackTraceBeginM();
    FitnessNMSE::Handle fitness = new FitnessNMSE(outputs);
    for(unsigned i=0; i<fitness->size(); ++i) {
        (*fitness)[i].nmse = std::numeric_limits<double>::infinity();
        (*fitness)[i].mse = std::numeric_limits<double>::infinity();
    }
    fitness->mNMSE = std::numeric_limits<double>::infinity();
    fitness->setValid();
    return fitness;
    Beagle_StackTraceEndM("FitnessNMSE::Handle SinsGP::void FitnessNMSE::unstable()");
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
    //std::cout << std::endl << mNMSE << ((mNMSE > rightFitness.mNMSE) ? " > " : " < ") << rightFitness.mNMSE;
    if(isValid()==false) {
      //  std::cout << " [1]";
        return true;
    }
    if(!rightFitness.isValid()) {
        //std::cout << " [2]";
        return false;
    }
    //std::cout << " [3]";
    return mNMSE > rightFitness.mNMSE;
    Beagle_StackTraceEndM("bool SinsGP::FitnessNMSE::isLess(const Object& inRightObj) const");

}

void FitnessNMSE::write(PACC::XML::Streamer& ioStreamer, bool inIndent) const
{
    Beagle_StackTraceBeginM();
    ioStreamer.openTag("Fitness", inIndent);
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
