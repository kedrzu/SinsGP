#ifndef SINSGP_FITNESSCORELATION_H
#define SINSGP_FITNESSCORELATION_H

#include "Fitness.h"

namespace SinsGP {

class FitnessCorelation : public Fitness<double>
{
public:
    typedef Beagle::AllocatorT<FitnessCorelation,SinsGP::Fitness<double>::Alloc> Alloc;
    typedef Beagle::PointerT<FitnessCorelation,SinsGP::Fitness<double>::Handle> Handle;
    typedef Beagle::ContainerT<FitnessCorelation,SinsGP::Fitness<double>::Bag> Bag;

    explicit FitnessCorelation(unsigned outputs);
    explicit FitnessCorelation();
    virtual ~FitnessCorelation() { }

    virtual bool isEqual(const Beagle::Object& inRightObj) const;
    virtual bool isLess(const Beagle::Object& inRightObj) const;
    virtual void write(PACC::XML::Streamer& ioStreamer, bool inIndent=true) const;
    virtual void add(const std::vector<double>& output);
    virtual void commitData();
    virtual void evaluate();
    double getCorelation() { return mCorelation; }
    double getSign(unsigned i) { return mSign[i]; }
protected:
    virtual void postSetData();
    std::vector<Signal> mSignals;
    std::vector<double> mSign;
    std::vector<double> mCorXTime;
    double mCorelation;
    unsigned mTime;
};
}

#endif // SINSGP_FITNESSCORELATION_H
