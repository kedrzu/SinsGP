
//#include "stdafx.h"
//#include "ContinousModelTesting.h"

//using namespace SinsGP;
//using namespace std;
//using namespace Beagle;
//ContinousModelTesting::ContinousModelTesting(Config& config, Beagle::GP::Individual& individual, const Data& data, GP::Context& context)
//    : ContinousModel(config, individual, data, context) {
//}

//void ContinousModelTesting::operator() (const StateType &x, StateType &dxdt, const double t)
//{

//    // szukamy nastpnego wiersza danych
//    while(mData[mCounter].t < t)
//        mCounter++;
//    bool first = mCounter == 0;
//    // aproksymacja danych uczcych prostoktami w przd
//    if(mRectangular) {
//        // pobranie wiersza danych
//        const DataRow& row = mData[first ? 0 : mCounter-1];

//        // iteracja po wejsciach
//        for(unsigned k=0; k<mInputs; k++) {
//            mConfig.getInputs(k)->setValue(Double(row.u[k]));
//        }

//        // iteracja po zmiennych stanu do zapisu aktualnego stanu
//        for(unsigned k=0; k<mOrder; ++k) {
//                mConfig.getStateVar(k)->setValue(Double(x[k]));
//        }

//        // iteracja po wyjœciach dla zmiennych obserwowalnych
//        for(unsigned k=0; k<mObservables; ++k) {
//            double error = row.y[k]-(double)x[mConfig.getObservables(k)];
//            dxdt[k+mOrder] = error*error;
//        }

//        // iteracja po pozosta³ych wyjœciach
//        for(unsigned k=0; k<mOutTrees; ++k) {
//            Double result; // deklaracja zmiennej do zapisu wyniku
//            mIndividual[k]->interpret(result, mContext); // odczyt wyniku obliczeñ dla drzewa
//            double error = row.y[k+mObservables]-(double)result;
//            dxdt[k+mOrder+mObservables] = error*error;
//        }

//        // iteracja po zmiennych stanu do odczytu pochodnej
//        for(unsigned k=0, trees=0; k<mOrder; ++k) {
//            // je¿eli jest pochodn¹ i jest obserwowalna to bierzemy wartoœæ z modelu
//            if(mConfig.isDerivative(k)) {
//                dxdt[k] = x[k-1];
//            }
//            else {
//                Double state;
//                mIndividual[trees+mOutTrees]->interpret(state, mContext);
//                dxdt[k] = (double)state;
//                ++trees;
//            }
//        }
//    }
//    /*
//    // aproksymacja danych uczcych trapezami
//    else {
//        // pobranie wiersza danych
//        const DataRow& row1 = mData[first ? 0 : mCounter-1];
//        const DataRow& row2 = mData[first ? 1 : mCounter];
//        double weight1 = (t-row1.t)/(row2.t - row1.t);
//        double weight2 = 1-weight1;

//        // iteracja po wejciach
//        for(unsigned k=0; k<mInputs; k++) {
//            double in = row1.u[k]*weight1 + row2.u[k]*weight2;
//            mConfig.getInputs(k)->setValue(Double(in));
//        }

//        // iteracja po zmiennych stanu do zapisu aktualnego stanu
//        for(unsigned k=0; k<mOrder; k++) {
//            mConfig.getStateVar(k)->setValue(Double(x[k]));
//        }

//        // iteracja po zmiennych stanu do odczytu pochodnej
//        for(unsigned k=0; k<mOrder; k++) {
//            Double result;
//            mIndividual[k]->interpret(result, mContext); // odczyt wyniku oblicze dla drzewa
//            dxdt[k] = result;
//        }

//        // obliczenie bdu
//        for(unsigned k=0; k<mOutputs; k++) {
//            Double result;
//            double out = row1.y[k]*weight1 + row2.y[k]*weight2;
//            mIndividual[k+mOrder]->interpret(result, mContext); // odczyt wyniku oblicze dla drzewa
//            // normalizacja bdu
//            dxdt[k+mOrder] = abs(out - (double)result)/mData.getOutStats(k).RMS;
//        }
//    }
//    */
//}
