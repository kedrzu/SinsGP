#ifndef SINSGP_FITNESSNMSE_H
#define SINSGP_FITNESSNMSE_H

#include "Fitness.h"

namespace SinsGP {

struct NMSE {
    double mse;
    double nmse;
    NMSE() : mse(0), nmse(0) {}
};

class FitnessNMSE : public Fitness<NMSE>
{
public:

    typedef Beagle::AllocatorT<FitnessNMSE,SinsGP::Fitness<NMSE>::Alloc> Alloc;
    typedef Beagle::PointerT<FitnessNMSE,SinsGP::Fitness<NMSE>::Handle> Handle;
    typedef Beagle::ContainerT<FitnessNMSE,SinsGP::Fitness<NMSE>::Bag> Bag;

    FitnessNMSE();
    explicit FitnessNMSE(unsigned outputs);
    virtual ~FitnessNMSE() { }

    virtual bool isEqual(const Beagle::Object& inRightObj) const;
    virtual bool isLess(const Beagle::Object& inRightObj) const;
    virtual void write(PACC::XML::Streamer& ioStreamer, bool inIndent=true) const;

    double getNMSE() const { return mNMSE; }
    virtual void add(std::vector<double> output);
    virtual void commitData();
    virtual void evaluate();

protected:
    double mNMSE;
    std::vector<double> mPow2SumBuffer;
    std::vector<double> mPow2Sum;
    std::vector<double> mPow2SumNorm;
    unsigned mStep;
    unsigned mTime;
};
}
#endif // SINSGP_FITNESSNMSE_H
