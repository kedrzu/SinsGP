#include "stdafx.h"
#include "MilestoneWriteOp.h"

using namespace SinsGP;
using namespace Beagle;
using namespace boost::filesystem;

SinsGP::MilestoneWriteOp::MilestoneWriteOp(Config& config, Status& status)
    : mConfig(&config), mStatus(&status)
{
}

void SinsGP::MilestoneWriteOp::operate(Deme &ioDeme, Context &ioContext) {
    Beagle_StackTraceBeginM();
    Beagle_StackTraceEndM("void MilestoneWriteOp::operate(Deme& ioDeme, Context& ioContext)");
}
