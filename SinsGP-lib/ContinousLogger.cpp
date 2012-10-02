#include "stdafx.h"
#include "SinsGP.h"

using namespace SinsGP;
using namespace std;
using namespace Beagle;

ContinousLogger::ContinousLogger(GP::Individual &individual, Result& result, ContinousModel::StateType& x, GP::Context &context)
    :ContinousModel(result.getConfig(), individual, result.getData(), context), mX(x), mResult(result)
{
}

void ContinousLogger::operator ()(const StateType& x, const double t)
{
    // zapisujemy aktualny stan do bufora
    mX = x;

    // szukamy nastêpnego wiersza danych
    while(mData[mCounter].t < t)
        mCounter++;
	bool first = mCounter == 0;
    // wiersz danych
    ResultRow resultRow(mInputs, mOutputs, mOrder);
    // aproksymacja danych ucz¹cych prostok¹tami w przód
    if(mRectangular) {
        // pobranie wiersza danych
		const DataRow& row = mData[first ? 0 : mCounter-1];
        resultRow.t = t;
        resultRow.u = row.u;
        resultRow.y = row.y;

        // iteracja po wejœciach
        for(unsigned k=0; k<mInputs; k++) {
            mConfig.getInputs(k)->setValue(Double(row.u[k]));
        }

        // iteracja po zmiennych stanu
        for(unsigned k=0; k<mOrder; k++) {
            mConfig.getStateVar(k)->setValue(Double(x[k]));
            resultRow.x[k] = x[k];
        }

        // iteracja po wyjœciach dla zmiennych obserwowalnych
        for(unsigned k=0; k<mObservables; ++k) {
            double error = row.y[k]-(double)x[mConfig.getObservables(k)];
            resultRow.e[k] = error;
            resultRow.yModel[k] = (double)x[mConfig.getObservables(k)];
        }

        // iteracja po pozosta³ych wyjœciach
        for(unsigned k=0; k<mOutTrees; ++k) {
            Double result; // deklaracja zmiennej do zapisu wyniku
            mIndividual[k]->interpret(result, mContext); // odczyt wyniku obliczeñ dla drzewa
            double error = row.y[k+mObservables]-(double)result;
            resultRow.e[k] = error;
            resultRow.yModel[k] = (double)result;
        }
    }
    /*
    // aproksymacja danych ucz¹cych trapezami
    else {
        // pobranie wiersza danych
		const DataRow& row1 = mData[first ? 0 : mCounter-1];
		const DataRow& row2 = mData[first ? 1 : mCounter];
        double weight1 = (t-row1.t)/(row2.t - row1.t);
        double weight2 = 1-weight1;

        resultRow.t = t;

        // iteracja po wejœciach
        for(unsigned k=0; k<mInputs; k++) {
            double in = row1.u[k]*weight1 + row2.u[k]*weight2;
            mConfig.getInputs(k)->setValue(Double(in));
            resultRow.u[k] = in;
        }

        // iteracja po zmiennych stanu do zapisu aktualnego stanu
        for(unsigned k=0; k<mOrder; k++) {
            mConfig.getStateVar(k)->setValue(Double(x[k]));
            resultRow.x[k] = x[k];
        }

        // obliczenie b³êdu
        for(unsigned k=0; k<mOutputs; k++) {
            Double result;
            double out = row1.y[k]*weight1 + row2.y[k]*weight2;
            mIndividual[k+mOrder]->interpret(result, mContext); // odczyt wyniku obliczeñ dla drzewa
            double e = out - (double)result;
            resultRow.y[k] = out;
            resultRow.yModel[k] = (double)result;
            resultRow.e[k] = e;
        }
    }
    */
    // dodanie wiersza do wyników
    mResult.push_back(resultRow);
}
