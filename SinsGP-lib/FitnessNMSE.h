#ifndef FITNESSNMSE_H
#define FITNESSNMSE_H

#include <utility>

namespace SinsGP {

struct NMSE {
    double mse;
    double nmse;
    NMSE() : mse(0), nmse(0) {}
};

class FitnessNMSE : public Beagle::Fitness, public std::vector<NMSE>
{
public:

    typedef Beagle::AllocatorT<FitnessNMSE,Fitness::Alloc> Alloc;
    typedef Beagle::PointerT<FitnessNMSE,Fitness::Handle> Handle;
    typedef Beagle::ContainerT<FitnessNMSE,Fitness::Bag> Bag;

    FitnessNMSE();
    explicit FitnessNMSE(unsigned outputs);
    virtual ~FitnessNMSE() { }

    virtual bool isEqual(const Beagle::Object& inRightObj) const;
    virtual bool isLess(const Beagle::Object& inRightObj) const;
    virtual void write(PACC::XML::Streamer& ioStreamer, bool inIndent=true) const;

    double getNMSE() const { return mNMSE; }
    void evaluate();
    static FitnessNMSE::Handle unstable(unsigned outputs);

protected:
    double mNMSE;
};
}
#endif // FITNESSNMSE_H
