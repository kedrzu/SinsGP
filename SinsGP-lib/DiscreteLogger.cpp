#include "stdafx.h"
#include "DiscreteLogger.h"

using namespace SinsGP;
using namespace std;
using namespace Beagle;

DiscreteLogger::DiscreteLogger(GP::Individual& individual, SinsGP::Result &result, Beagle::GP::Context &context)
:DiscreteModel(result.getConfig(), individual, result.getData(), context), mResult(result)
{
}

DiscreteLogger::~DiscreteLogger(){}

DiscreteModel::Error DiscreteLogger::integrate() {
	Error errorSum(mOutputs, 0);
	ResultRow resultRow(mInputs, mOutputs, mOrder);

	// warunki pocz¹tkowe x=0
	// wektor ma maksymalny mo¿liwy rozmiar, ale wszystkie zmienne stanu
	// które wykraczaj¹ poza rz¹d danego osobnika maj¹ wartoœæ 1
	// poniewa¿ ka¿dy z osobników ma dostêp do tych zmiennych, nawet je¿eli nie ma
	// odpowiedniego rzêdu
	vector<Double> x(mMaxOrder, 0);
    for(unsigned j=mOrder; j<x.size(); ++j) {
		x[j] = 1;
		mConfig.getStateVar(j)->setValue(Double(x[j]));
	}

	// iteracja po wierszach w jednym przypadku ucz¹cym
    for(unsigned j=0; j<mData.size(); ++j) {

		// pobranie wiersza danych
		const DataRow& row = mData[j];

		// iteracja po wejœciach
        for(unsigned k=0; k<mInputs; ++k) {
			mConfig.getInputs(k)->setValue(Double(row.u[k]));
		}

		// iteracja po zmiennych stanu do zapisu aktualnego stanu
        for(unsigned k=0; k<mOrder; ++k) {
			mConfig.getStateVar(k)->setValue(Double(x[k]));
		}

		// zapis danych do logów
		resultRow.t = row.t; // zapis czasu
		resultRow.u = row.u; // zapis wejœcia
        for(unsigned k=0; k<mOrder; ++k) {
			resultRow.x[k] = x[k]; // zapis stanu
		}
		resultRow.y = row.y; // zapis wyjœcia

        // iteracja po wyjœciach dla zmiennych obserwowalnych
        for(unsigned k=0; k<mObservables; ++k) {
            double error = row.y[k]-(double)x[mConfig.getObservables(k)];
            resultRow.yModel[k] = x[mConfig.getObservables(k)]; // zapis wyniku predykcji
            resultRow.e[k] = error; // zapis b³êdu
            errorSum[k] += error*error;
        }

        // iteracja po pozosta³ych wyjœciach
        for(unsigned k=0; k<mOutTrees; ++k) {
            Double result; // deklaracja zmiennej do zapisu wyniku
            mIndividual[k]->interpret(result, mContext); // odczyt wyniku obliczeñ dla drzewa
            int l = k+mObservables;
            double error = row.y[l]-(double)result;
            resultRow.yModel[l] = (double)result; // zapis wyniku predykcji
            resultRow.e[l] = error; // zapis b³êdu
            errorSum[l] += error*error;
        }

        // iteracja po zmiennych stanu do odczytu nastêpnego stanu
        for(unsigned k=0, trees=0; k<mOrder; ++k) {
            // je¿eli jest pochodn¹ i jest obserwowalna to bierzemy wartoœæ z modelu
            if(mConfig.isDerivative(k)) {
                x[k] += x[k-1];
            } else {
                Double state;
                mIndividual[trees+mOutTrees]->interpret(state, mContext);
                x[k] += (double)state;
                ++trees;
            }
        }

        // dodanie wyniku do logów
		mResult.push_back(resultRow);
	}
	return errorSum;
}
