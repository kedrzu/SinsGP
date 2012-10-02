#ifndef STATSCALCFITNESSNMSEOP_H
#define STATSCALCFITNESSNMSEOP_H

#include "Config.h"
#include "FitnessNMSE.h"

namespace SinsGP {
class StatsCalcFitnessNMSEOp : public Beagle::StatsCalculateOp {

public:

  typedef Beagle::AllocatorT<StatsCalcFitnessNMSEOp,Beagle::StatsCalculateOp::Alloc> Alloc;
  typedef Beagle::PointerT<StatsCalcFitnessNMSEOp,Beagle::StatsCalculateOp::Handle> Handle;
  typedef Beagle::ContainerT<StatsCalcFitnessNMSEOp,Beagle::StatsCalculateOp::Bag> Bag;

  explicit StatsCalcFitnessNMSEOp(Config& config, std::string inName="StatsCalcFitnessNMSEOp");
  virtual ~StatsCalcFitnessNMSEOp() { }

  virtual void calculateStatsDeme(Beagle::Stats& outStats, Beagle::Deme& ioDeme, Beagle::Context& ioContext) const;

  protected:
    Config* mConfig;
};

}


#endif // STATSCALCFITNESSNMSEOP_H
