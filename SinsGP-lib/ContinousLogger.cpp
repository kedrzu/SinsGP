#include "stdafx.h"
#include "SinsGP.h"

using namespace SinsGP;
using namespace std;
using namespace Beagle;

ContinousLogger::ContinousLogger(Config& config, GP::Individual& individual, const Data& data, GP::Context& context)
    :ContinousModel(config, individual, data, context), mResultRow(mInputs, mOutputs, mOrder)
{
}

void ContinousLogger::operator ()(const StateType &x, StateType &dxdt, const double t)
{

    // pobranie wiersza danych
    const DataRow& row = mData[mStep];

    // wiersz danych
    mResultRow = ResultRow(mInputs, mOutputs, mOrder);
    mResultRow.t = t;
    mResultRow.u = row.u;
    mResultRow.y = row.y;

    // iteracja po wejsciach
    for(unsigned k=0; k<mInputs; k++) {
        mConfig.getInputs(k)->setValue(Double(row.u[k]));
    }

    // iteracja po zmiennych stanu do zapisu aktualnego stanu
    for(unsigned k=0; k<mOrder; ++k) {
            mConfig.getStateVar(k)->setValue(Double(x[k]));
            mResultRow.x[k] = x[k];
    }

    // iteracja po wyjœciach dla zmiennych obserwowalnych
    for(unsigned k=0; k<mObservables; ++k) {
        double error = row.y[k]-(double)x[mConfig.getObservables(k)];
        dxdt[k+mOrder] = error*error;
        mResultRow.e[k] = error;
        mResultRow.yModel[k] = (double)x[mConfig.getObservables(k)];
    }

    // iteracja po pozosta³ych wyjœciach
    for(unsigned k=0; k<mOutTrees; ++k) {
        Double result; // deklaracja zmiennej do zapisu wyniku
        mIndividual[k]->interpret(result, mContext); // odczyt wyniku obliczeñ dla drzewa
        double error = row.y[k+mObservables]-(double)result;
        dxdt[k+mOrder+mObservables] = error*error;
        mResultRow.e[k] = error;
        mResultRow.yModel[k] = (double)result;
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
