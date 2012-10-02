#ifndef CONTINOUSMODELTESTING_H
#define CONTINOUSMODELTESTING_H

#include "ContinousModel.h"

namespace SinsGP {
class ContinousModelTesting : public ContinousModel
{
public:
    ContinousModelTesting(Config& config, Beagle::GP::Individual& individual, const Data& data, Beagle::GP::Context& context);
    void operator()(const StateType &x, StateType &dxdt, const double t);
};
}

#endif // CONTINOUSMODELTESTING_H
