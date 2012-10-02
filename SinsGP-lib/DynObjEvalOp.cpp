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
    double time = 0;
    // błędy
    std::vector<NMSE> error(mOutputs);

    try {
        // iteracja po wszyskich przypadkach uczących (plikach z danymi)
        for(unsigned int i=0; i<mData.size(); i++) {

            Data& data = *mData[i];
            time += data.getDuration();

            // model jest ciagły
            if(mConfig.getContinous()) {
                // tworzymy model
                ContinousModelConstStep model(mConfig, inIndividual, data, ioContext);
                // stan początkowy
                ContinousModelConstStep::StateType x(order+mOutputs, 0);
                // ilość kroków w jednym przypadku
                unsigned steps = data.size();
                // czas próbkowania
                double ts = mConfig.getSamplingTime();
                // czas
                double t = 0;
                // stepper
                runge_kutta4< ContinousModelConstStep::StateType > stepper;
                // całkowanie
                for(unsigned i=0; i<steps; ++i) {
                    stepper.do_step(model, x, t, ts);
                    t += ts;
                    model.nextStep();
                }

                for(unsigned j=0; j<mOutputs; j++) {
                    error[j].mse += x[j+order];
                    error[j].nmse += x[j+order] / data.getOutStats(j).RMS;
                }


//                // tworzymy model
//                ContinousModel model(mConfig, inIndividual, data, ioContext);
//                // stan początkowy
//                ContinousModel::StateType xInit(order+mOutputs, 0);
//                ContinousModel::StateType x(order+mOutputs, 0);
//                integrate(model, xInit, (double)0, data.getDuration(), data[1].t-data[0].t, ContinousObserver(mConfig, x));

//                for(unsigned j=0; j<mOutputs; j++) {
//                    error[j].mse += x[j+order];
//                    error[j].nmse += x[j+order] / data.getOutStats(j).RMS;
//                }

            }
            // model jest dyskretny
            else {
                // tworzymy model
                DiscreteModel model(mConfig, inIndividual, *mData[i], ioContext);
                // uruchamiamy go
                vector<double> e = model.integrate();
                for(unsigned j=0; j<mOutputs; j++) {
                    error[j].mse += e[j];
                    error[j].nmse += e[j] / data.getOutStats(j).RMS;
                }
            }
        }
    } catch (ContinousModel::Exception e) {
/*
            PACC::Date date; // obecna data
            switch(e) {
                case ContinousModel::Unstable:
                    cout << endl << date.get("%d-%m-%Y %H-%M-%S") << "\t NIESTABILNY";
                    break;
                case ContinousModel::TimeExceeded:
                    cout << endl << date.get("%d-%m-%Y %H-%M-%S") << "\t PRZEKROCZONO CZAS";
                    break;
                default:
                    cout << endl << date.get("%d-%m-%Y %H-%M-%S") << "\t BLAD";
            }
*/
        FitnessNMSE::Handle fitness = FitnessNMSE::unstable(mOutputs);
        return fitness;
    }

    // funkcja przystosowania
    FitnessNMSE::Handle fitness = new FitnessNMSE(mOutputs);

    // uśrednienie błędu po czasie
    for(int i=0; i<mOutputs; ++i) {
        (*fitness)[i].mse = error[i].mse / time;
        (*fitness)[i].nmse = error[i].nmse / time;
    }
    fitness->evaluate();
    return fitness;
}

