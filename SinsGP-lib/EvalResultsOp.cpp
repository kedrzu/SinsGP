#include "stdafx.h"
#include "SinsGP.h"
#include <boost/numeric/odeint.hpp>

using namespace std;
using namespace Beagle;
using namespace SinsGP;
using namespace boost::filesystem;
using namespace boost::numeric::odeint;

EvalResultsOp::EvalResultsOp(Config& config, Data::Bag data, Data::Bag test, SinsGP::Stats* stats) :
    GP::EvaluationOp("DynObjEvalOp"), mConfig(config), mData(data), mTest(test),
    mInputs(config.getInputs().size()), mOutputs(config.getOutputs().size()),
    mMaxOrder(config.getMaxOrder()), mTesting(false), mStats(stats)
{
}

EvalResultsOp::~EvalResultsOp(void)
{
}


// Funkcja przystosowania
Fitness::Handle EvalResultsOp::evaluate(GP::Individual& inIndividual, GP::Context& ioContext)
{
    // odczyt rzêdu modelu osobnika
    int order = inIndividual.size() + mConfig.getDerivatives().size() + mConfig.getObservables().size() - mOutputs;
    // czas wszystkich przypadków danych
    double time = 0;
    // b³êdy
    std::vector<NMSE> error(mOutputs);

    // czy przetwarzane bêd¹ dane ucz¹ce, czy testowe
    Data::Bag& dataSet = mTesting ? mTest : mData;

        // iteracja po wszyskich przypadkach ucz¹cych (plikach z danymi)
        for(unsigned int i=0; i<dataSet.size(); i++) {

            Data& data = *dataSet[i];
            time += data.getDuration();

            // obiekt do zapisu danych uczenia
            Result result(mConfig, order, data, mResult->getStatus(), mTesting);

            // model jest ciag³y
            if(mConfig.getContinous()) {
                // tworzymy model
                ContinousLogger model(mConfig, inIndividual, data, ioContext);
                // stan pocz¹tkowy
                ContinousLogger::StateType x(order+mOutputs, 0);
                // iloœæ kroków w jednym przypadku
                unsigned steps = data.size();
                // czas próbkowania
                double ts = mConfig.getSamplingTime();
                // czas
                double t = 0;
                // stepper
                runge_kutta4< ContinousModel::StateType > stepper;
                // ca³kowanie
                for(unsigned i=0; i<steps; ++i) {
                    stepper.do_step(model, x, t, ts);
                    t += ts;
                    model.nextStep();
                    result.push_back(model.getResultRow());
                }

                for(unsigned j=0; j<mOutputs; j++) {
                    error[j].mse += x[j+order];
                    error[j].nmse += x[j+order] / data.getOutStats(j).RMS;
                }
            }
            // model jest dyskretny
            else {
                // tworzymy logger (model z zapisywaniem wyników)
                DiscreteLogger logger(inIndividual, result, ioContext);

                vector<double> e = logger.integrate();
                for(unsigned j=0; j<mOutputs; j++) {
                    error[j].mse += e[j];
                    error[j].nmse += e[j] / data.getOutStats(j).RMS;
                }
            }
            // zapis danych do plików
            result.exec();
        }

    // funkcja przystosowania
    FitnessNMSE::Handle fitness = new FitnessNMSE(mOutputs);

    // uœrednienie b³êdu po czasie
    for(unsigned i=0; i<mOutputs; ++i) {
        (*fitness)[i].mse = error[i].mse / time;
        (*fitness)[i].nmse = error[i].nmse / time;
    }
    fitness->evaluate();

    return fitness;

}

void EvalResultsOp::logResults(ResultModel& result, Beagle::GP::System::Handle ioSystem)
{
    mResult = &result;
    SinsGP::Stats::Row row;
    row.run = result.getStatus().getRun();
    row.model = result.getStatus().getModel();
    FitnessNMSE::Handle fitness;
    mTesting = false;
    fitness = castHandleT<FitnessNMSE>(test(result.getIndividual(), ioSystem));
    row.training = *fitness;
    mTesting = true;
    fitness = castHandleT<FitnessNMSE>(test(result.getIndividual(), ioSystem));
    row.testing = *fitness;
    mTesting = false;
    mStats->addRow(row);
}
