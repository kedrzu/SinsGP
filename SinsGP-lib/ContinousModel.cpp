#include "stdafx.h"
#include "ContinousModel.h"

using namespace SinsGP;
using namespace std;
using namespace Beagle;

ContinousModel::ContinousModel(Config& config, Beagle::GP::Individual& individual, const Data& data, GP::Context& context)
    : mConfig(config),
      mIndividual(individual),
      mData(data),
      mContext(context),
      mInputs(config.getInputs().size()),
      mOutputs(config.getOutputs().size()),
      mMaxOrder(config.getMaxOrder()),
      mObservables(config.getObservables().size()),
      mDerivatives(config.getDerivatives().size())
{

    // odczyt rzêdu modelu osobnika
    mOrder = individual.size() - mOutputs + mObservables + mDerivatives;
    // czy osobnik ma odpowiedni¹ liczbê drzew
    if(mOrder > mConfig.getMaxOrder() || mOrder < mConfig.getMinOrder()) {
        std::string e = "SinsGP::DiscreteModel: Osobnik zawiera " + int2str(mIndividual.size()) + " drzew, a powinno byæ od " + int2str((mOutputs + mConfig.getMinOrder() - mConfig.getObservables().size() - mConfig.getDerivatives().size())) + " do " + int2str((mOutputs + mConfig.getMaxOrder() - mConfig.getObservables().size() - mConfig.getDerivatives().size()));
        throw runtime_error(e);
    }

    mOutTrees = mOutputs - mObservables;
    mStateTrees = mOrder - mDerivatives;

    // pocztkowy stan
    for(unsigned k=0; k<mMaxOrder; k++) {
        mConfig.getStateVar(k)->setValue(Double(0));
    }
}

ContinousModel::~ContinousModel(void)
{
}

void ContinousModel::operator() (const StateType &x, StateType &dxdt, const double t)
{
    // iteracja po zmiennych stanu do zapisu aktualnego stanu
    for(unsigned k=0; k<mOrder; ++k) {
        mConfig.getStateVar(k)->setValue(Double(x[k]));
    }

    // iteracja po zmiennych stanu do odczytu pochodnej
    for(unsigned k=0, trees=0; k<mOrder; ++k) {
        // je¿eli jest pochodn¹ i jest obserwowalna to bierzemy wartoœæ z modelu
        if(mConfig.isDerivative(k)) {
            dxdt[k] = x[k-1];
        }
        else {
            Double state;
            mIndividual[trees+mOutTrees]->interpret(state, mContext);
            dxdt[k] = (double)state;
            ++trees;
        }
    }
}

void ContinousModel::setInputs(unsigned step) {
    // iteracja po wejsciach
    for(unsigned k=0; k<mInputs; k++) {
        mConfig.getInputs(k)->setValue(Double(mData.getInputs(k)[step]));
    }
}

vector<double> ContinousModel::getOutput(StateType& x) {
    vector<double> out(mOutputs);
    // iteracja po wyjœciach dla zmiennych obserwowalnych
    for(unsigned k=0; k<mObservables; ++k) {
        out[k] = (double)x[mConfig.getObservables(k)];
    }

    // iteracja po pozosta³ych wyjœciach
    for(unsigned k=0; k<mOutTrees; ++k) {
        Double y; // deklaracja zmiennej do zapisu wyniku
        mIndividual[k]->interpret(y, mContext); // odczyt wyniku obliczeñ dla drzewa
        out[k+mObservables] = (double)y;
    }
    return out;
}
