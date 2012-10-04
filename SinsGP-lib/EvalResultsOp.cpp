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

    // odczyt rz�du modelu osobnika
    unsigned order = inIndividual.size() + mConfig.getDerivatives().size() + mConfig.getObservables().size() - mOutputs;
    // czas wszystkich przypadk�w danych
    unsigned time = 0;
    // b��dy
    //std::vector<NMSE> error(mOutputs);
    double correlation = 0;

    // czy przetwarzane b�d� dane ucz�ce, czy testowe
    Data::Bag& dataSet = mTesting ? mTest : mData;

        // iteracja po wszyskich przypadkach ucz�cych (plikach z danymi)
        for(unsigned int i=0; i<dataSet.size(); i++) {

        Data& data = *dataSet[i];

        // obiekt do zapisu danych uczenia
        Result result(mConfig, order, data, mResult->getStatus(), mTesting);

        // model jest ciag�y
        if(mConfig.getContinous()) {
            // tworzymy model
            ContinousModel model(mConfig, inIndividual, data, ioContext);
            // stan pocz�tkowy
            ContinousModel::StateType x(order, 0);
            // ilo�� krok�w w jednym przypadku
            unsigned steps = data.getRows();
            // czas pr�bkowania
            double ts = mConfig.getSamplingTime();
            // czas
            double t = 0;
            // zapis sygna��w wyj�ciowych
            vector<Signal> outputs(mOutputs);
            for(unsigned i=0; i<mOutputs; ++i) {
                outputs[i] = Signal(steps);
            }
            // stepper
            runge_kutta4< ContinousModel::StateType > stepper;
            // ca�kowanie
            for(unsigned j=0; j<steps; ++j) {
                // zapis do wynik�w
                ResultRow row(mConfig.getInputs().size(), mOutputs, order);
                row.t = t;
                for(unsigned k=0; k<mConfig.getInputs().size(); ++k)
                    row.u[k] = data.getInputs(k)[j];
                for(unsigned k=0; k<order; ++k)
                    row.x[k] = x[k];

                model.setInputs(j);
                vector<double> out = model.getOutput(x);
                // zapis sygna��w wyj�ciowych
                for(unsigned k=0; k<mOutputs; ++k) {
                    outputs[k].push_back(out[k]);
                    row.y[k] = data.getOutputs(k)[j];
                    row.yModel[k] = out[k];
                }

                stepper.do_step(model, x, t, ts);

                result.push_back(row);

                // nast�pny krok
                t += ts;
            }

            time += steps;
            for(unsigned j=0; j<mOutputs; ++j) {
                outputs[j].evalMean();
                correlation += data.getOutputs(j).correlation(outputs[j])*steps;
            }

//            if(model.isZero()) {
//                return FitnessNMSE::unstable(mOutputs);
//            }

//            for(unsigned j=0; j<mOutputs; j++) {
//                error[j].mse += x[j+order];
//                error[j].nmse += x[j+order] / data.getOutStats(j).RMS;
//            }

        }
        result.exec();
    }

    // funkcja przystosowania
//    FitnessNMSE::Handle fitness = new FitnessNMSE(mOutputs);

//    // u�rednienie b��du po czasie
//    for(int i=0; i<mOutputs; ++i) {
//        (*fitness)[i].mse = error[i].mse / time;
//        (*fitness)[i].nmse = error[i].nmse / time;
//    }
//    fitness->evaluate();
//    return fitness;

    return new FitnessSimple(correlation/time);

//    // odczyt rz�du modelu osobnika
//    int order = inIndividual.size() + mConfig.getDerivatives().size() + mConfig.getObservables().size() - mOutputs;
//    // czas wszystkich przypadk�w danych
//    double time = 0;
//    // b��dy
//    std::vector<NMSE> error(mOutputs);

//    // czy przetwarzane b�d� dane ucz�ce, czy testowe
//    Data::Bag& dataSet = mTesting ? mTest : mData;

//        // iteracja po wszyskich przypadkach ucz�cych (plikach z danymi)
//        for(unsigned int i=0; i<dataSet.size(); i++) {

//            Data& data = *dataSet[i];
//            time += data.getDuration();

//            // obiekt do zapisu danych uczenia
//            Result result(mConfig, order, data, mResult->getStatus(), mTesting);

//            // model jest ciag�y
//            if(mConfig.getContinous()) {
//                // tworzymy model
//                ContinousLogger model(mConfig, inIndividual, data, ioContext);
//                // stan pocz�tkowy
//                ContinousLogger::StateType x(order+mOutputs, 0);
//                // ilo�� krok�w w jednym przypadku
//                unsigned steps = data.size();
//                // czas pr�bkowania
//                double ts = mConfig.getSamplingTime();
//                // czas
//                double t = 0;
//                // stepper
//                runge_kutta4< ContinousModel::StateType > stepper;
//                // ca�kowanie
//                for(unsigned i=0; i<steps; ++i) {
//                    stepper.do_step(model, x, t, ts);
//                    t += ts;
//                    model.nextStep();
//                    result.push_back(model.getResultRow());
//                }

//                for(unsigned j=0; j<mOutputs; j++) {
//                    error[j].mse += x[j+order];
//                    error[j].nmse += x[j+order] / data.getOutStats(j).RMS;
//                }
//            }
//            // model jest dyskretny
//            else {
//                // tworzymy logger (model z zapisywaniem wynik�w)
//                DiscreteLogger logger(inIndividual, result, ioContext);

//                vector<double> e = logger.integrate();
//                for(unsigned j=0; j<mOutputs; j++) {
//                    error[j].mse += e[j];
//                    error[j].nmse += e[j] / data.getOutStats(j).RMS;
//                }
//            }
//            // zapis danych do plik�w
//            result.exec();
//        }

//    // funkcja przystosowania
//    FitnessNMSE::Handle fitness = new FitnessNMSE(mOutputs);

//    // u�rednienie b��du po czasie
//    for(unsigned i=0; i<mOutputs; ++i) {
//        (*fitness)[i].mse = error[i].mse / time;
//        (*fitness)[i].nmse = error[i].nmse / time;
//    }
//    fitness->evaluate();

//    return fitness;

}

void EvalResultsOp::logResults(ResultModel& result, Beagle::GP::System::Handle ioSystem)
{
    mResult = &result;
    SinsGP::Stats::Row row;
    row.run = result.getStatus().getRun();
    row.model = result.getStatus().getModel();
    //FitnessNMSE::Handle fitness;
    mTesting = false;
    test(result.getIndividual(), ioSystem);
    //fitness = castHandleT<FitnessNMSE>(test(result.getIndividual(), ioSystem));
    //row.training = *fitness;
    mTesting = true;
    test(result.getIndividual(), ioSystem);
    //fitness = castHandleT<FitnessNMSE>(test(result.getIndividual(), ioSystem));
    //row.testing = *fitness;
    mTesting = false;
    //mStats->addRow(row);
}
