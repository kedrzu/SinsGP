#include "stdafx.h"
#include "Status.h"

using namespace SinsGP;
Status::Status() : mRun(0), mModel(0)
{
    updatePaths();
}
