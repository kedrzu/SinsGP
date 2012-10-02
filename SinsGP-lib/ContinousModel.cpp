#include "stdafx.h"
#include "ContinousModel.h"

using namespace SinsGP;
using namespace std;
using namespace Beagle;

ContinousModel::ContinousModel(Config& config, Beagle::GP::Individual& individual, const Data& data, GP::Context& context)
    : mConfig(config), mIndividual(individual), mData(data), mStep(0),
      mInputs(config.getInputs().size()), mOutputs(config.getOutputs().size()),
      mMaxOrder(config.getMaxOrder()), mContext(context), mObservables(config.getObservables().size()), mDerivatives(config.getDerivatives().size())
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
    // pobranie wiersza danych
    const DataRow& row = mData[mStep];

    // iteracja po wejsciach
    for(unsigned k=0; k<mInputs; k++) {
        mConfig.getInputs(k)->setValue(Double(row.u[k]));
    }

    // iteracja po zmiennych stanu do zapisu aktualnego stanu
    for(unsigned k=0; k<mOrder; ++k) {
            mConfig.getStateVar(k)->setValue(Double(x[k]));
    }

    // iteracja po wyjœciach dla zmiennych obserwowalnych
    for(unsigned k=0; k<mObservables; ++k) {
        double error = row.y[k]-(double)x[mConfig.getObservables(k)];
        dxdt[k+mOrder] = error*error;
    }

    // iteracja po pozosta³ych wyjœciach
    for(unsigned k=0; k<mOutTrees; ++k) {
        Double result; // deklaracja zmiennej do zapisu wyniku
        mIndividual[k]->interpret(result, mContext); // odczyt wyniku obliczeñ dla drzewa
        double error = row.y[k+mObservables]-(double)result;
        dxdt[k+mOrder+mObservables] = error*error;
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

