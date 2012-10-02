#include "stdafx.h"
#include "SinsGP.h"

using namespace SinsGP;
using namespace std;
using namespace Beagle;

ContinousObserver::ContinousObserver(Config& config, ContinousModel::StateType& x)
: mConfig(config), mX(x), mOutputs(config.getOutputs().size()), mOrder(x.size()-mOutputs),
mSteps(0)
{
}

void ContinousObserver::operator ()(const ContinousModel::StateType& x, const double t)
{
	mSteps++;
    mX = x;

	// sprawdzanie, poprawno�ci dzia�ania algorytmu
	if(t>0) {
		if((double)mSteps/t > 1000)
			throw ContinousModel::TimeExceeded;
		// czy uk�ad jest stabilny
		for(int i=0; i<mOutputs; i++) {
			if(x[i+mOrder]/t > 1000)
				throw ContinousModel::Unstable;
		}
	}
}
