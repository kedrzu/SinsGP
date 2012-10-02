#ifndef MILESTONEWRITEOP_H
#define MILESTONEWRITEOP_H

#include "Config.h"
#include "Status.h"

namespace SinsGP {
class MilestoneWriteOp : public Beagle::MilestoneWriteOp
{
public:
    MilestoneWriteOp(Config& config, Status& status);
    virtual void operate(Beagle::Deme& ioDeme, Beagle::Context& ioContext);
private:
    Config* mConfig;
    Status* mStatus;
};
}

#endif // MILESTONEWRITEOP_H
