#include "stdafx.h"
#include "DynObjEvalOp.h"
#include "FitnessCorelation.h"
#include "ContinousModel.h"
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
Beagle::Fitness::Handle DynObjEvalOp::evaluate(GP::Individual& inIndividual, GP::Context& ioContext)
{
    // odczyt rzędu modelu osobnika
    int order = inIndividual.size() + mConfig.getDerivatives().size() + mConfig.getObservables().size() - mOutputs;
    // funkcja przystosowania
    FitnessCorelation::Handle fitness = new FitnessCorelation(mOutputs);

    // iteracja po wszyskich przypadkach uczących (plikach z danymi)
    for(unsigned int i=0; i<mData.size(); i++) {

        // przetwarzane dane
        Data& data = *mData[i];
        // ustawiamy dane
        fitness->setData(data);
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
        // stepper
        runge_kutta4< ContinousModel::StateType > stepper;
        // wykonanie symulacji
        for(unsigned j=0; j<steps; ++j) {
            // ustawiamy zmienne wejściowe
            model.setInputs(j);
            // zapisujemy wyjścia do funkcji przystosowania
            fitness->add(model.getOutput(x));
            // wykonujemy krok algorytmu numerycznego
            stepper.do_step(model, x, t, ts);
            // inkrementujemy czas
            t += ts;
        }
        // zatwierdzamy przypadek uczący
        fitness->commitData();
    }
    // wykonujemy końcowe obliczenia funkcji przystosowania
    fitness->evaluate();
    return fitness;
}

