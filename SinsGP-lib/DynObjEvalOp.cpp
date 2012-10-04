#include "stdafx.h"
#include "SinsGP.h"
#include <boost/numeric/odeint.hpp>

using namespace std;
using namespace Beagle;
using namespace SinsGP;
using namespace boost::filesystem;
using namespace boost::numeric::odeint;

DynObjEvalOp::DynObjEvalOp(Config& config, Data::Bag data) :
    GP::EvaluationOp("DynObjEvalOp"), mConfig(config), mData(data),
    mInputs(config.getInputs().size()), mOutputs(config.getOutputs().size()),
    mMaxOrder(config.getMaxOrder())
{
}

DynObjEvalOp::~DynObjEvalOp(void)
{
}


// Funkcja przystosowania
Fitness::Handle DynObjEvalOp::evaluate(GP::Individual& inIndividual, GP::Context& ioContext)
{
    // odczyt rzędu modelu osobnika
    int order = inIndividual.size() + mConfig.getDerivatives().size() + mConfig.getObservables().size() - mOutputs;
    // czas wszystkich przypadków danych
    unsigned time = 0;
    // błędy
    //std::vector<NMSE> error(mOutputs);
    double correlation = 0;

    // iteracja po wszyskich przypadkach uczących (plikach z danymi)
    for(unsigned int i=0; i<mData.size(); i++) {

        Data& data = *mData[i];

        // model jest ciagły
        if(mConfig.getContinous()) {
            // tworzymy model
            ContinousModel model(mConfig, inIndividual, data, ioContext);
            // stan początkowy
            ContinousModel::StateType x(order, 0);
            // ilość kroków w jednym przypadku
            unsigned steps = data.getRows();
            // czas próbkowania
            double ts = mConfig.getSamplingTime();
            // czas
            double t = 0;
            // zapis sygnałów wyjściowych
            vector<Signal> outputs(mOutputs);
            for(unsigned j=0; j<mOutputs; ++j) {
                outputs[j] = Signal(steps);
            }
            // stepper
            runge_kutta4< ContinousModel::StateType > stepper;
            // całkowanie
            for(unsigned j=0; j<steps; ++j) {
                model.setInputs(j);

                vector<double> out = model.getOutput(x);
                // zapis sygnałów wyjściowych
                for(unsigned k=0; k<mOutputs; ++k) {
                    outputs[k].push_back(out[k]);
                }

                stepper.do_step(model, x, t, ts);

//                cout << endl;
//                for(unsigned k=0; k<order; ++k) {
//                    cout << x[k] << "\t";
//                }
//                for(unsigned k=0; k<mOutputs; ++k) {
//                    cout << out[k] << "\t";
//                }

                // następny krok
                t += ts;
            }

            time += steps;
            for(unsigned j=0; j<mOutputs; ++j) {
                outputs[j].evalMean();
                double cor = data.getOutputs(j).correlation(outputs[j]);
                correlation += cor*steps;
                //cout << endl << outputs[j].getMean() << "\t" << data.getOutputs(j).getMean() << "\t" << cor;
            }

//            if(model.isZero()) {
//                return FitnessNMSE::unstable(mOutputs);
//            }

//            for(unsigned j=0; j<mOutputs; j++) {
//                error[j].mse += x[j+order];
//                error[j].nmse += x[j+order] / data.getOutStats(j).RMS;
//            }

        }
    }

    // funkcja przystosowania
//    FitnessNMSE::Handle fitness = new FitnessNMSE(mOutputs);

//    // uśrednienie błędu po czasie
//    for(int i=0; i<mOutputs; ++i) {
//        (*fitness)[i].mse = error[i].mse / time;
//        (*fitness)[i].nmse = error[i].nmse / time;
//    }
//    fitness->evaluate();
//    return fitness;

    return new FitnessSimple(correlation/time);
}

