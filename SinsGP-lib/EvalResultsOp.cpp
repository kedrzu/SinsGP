#include "stdafx.h"
#include "EvalResultsOp.h"
#include "FitnessCorelation.h"
#include "ContinousModel.h"
#include "Result.h"
#include <boost/numeric/odeint.hpp>

using namespace std;
using namespace Beagle;
using namespace SinsGP;
using namespace boost::filesystem;
using namespace boost::numeric::odeint;

EvalResultsOp::EvalResultsOp(Config& config, Data::Bag data, Data::Bag test, SinsGP::Stats* stats) :
    GP::EvaluationOp("DynObjEvalOp"), mConfig(config), mData(data), mTest(test),
    mInputs(config.getInputs().size()), mOutputs(config.getOutputs().size()),
    mMaxOrder(config.getMaxOrder()), mStats(stats)
{
}

EvalResultsOp::~EvalResultsOp(void)
{
}


// Funkcja przystosowania
Beagle::Fitness::Handle EvalResultsOp::evaluate(GP::Individual& inIndividual, GP::Context& ioContext)
{

    // odczyt rz�du modelu osobnika
    int order = inIndividual.size() + mConfig.getDerivatives().size() + mConfig.getObservables().size() - mOutputs;

    // funkcja przystosowania
    FitnessCorelation::Handle fitnessData = new FitnessCorelation(mOutputs);
    FitnessCorelation::Handle fitnessTest = new FitnessCorelation(mOutputs);
    // wyniki
    Result::Bag resultsData;
    Result::Bag resultsTest;
    // dane do liczenia �redniego odchylenia standardowego sygna��w
    vector<double> stdErrData(mOutputs);
    vector<double> stdErrTest(mOutputs);
    // liczba wierszy do liczenia �rednich
    double rowsData = 0;
    double rowsTest = 0;

    bool test = false;
    for(int k=0; k<2; ++k) {
        // czy przetwarzane b�d� dane ucz�ce, czy testowe
        Data::Bag& dataSet = test ? mTest : mData;
        Result::Bag& results = test ? resultsTest : resultsData;
        FitnessCorelation::Handle fitness = test ? fitnessTest : fitnessData;
        vector<double>& stdErr = test ? stdErrTest : stdErrData;
        double& rows = test ? rowsTest : rowsData;
        // iteracja po wszyskich przypadkach ucz�cych (plikach z danymi)
        for(unsigned int i=0; i<dataSet.size(); ++i) {
            // przetwarzane dane
            Data& data = *dataSet[i];
            // liczba wierszy
            rows += data.getRows();
            // odchylenie standardowe
            for(unsigned j=0; j<mOutputs; ++j) {
                stdErr[j] += data.getOutputs(j).getStats().stdErr*data.getRows();
            }
            // obiekt do zapisu danych uczenia
            results.push_back(new Result(mConfig, order, data, mResult->getStatus(), test));
            // ustawiamy dane
            fitness->setData(data);
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
            // stepper
            runge_kutta4< ContinousModel::StateType > stepper;
            // wykonanie symulacji
            for(unsigned j=0; j<steps; ++j) {
                // ustawiamy zmienne wej�ciowe
                model.setInputs(j);
                // zapisujemy wyj�cia do funkcji przystosowania i wyniku
                vector<double> out = model.getOutput(x);
                fitness->add(out);
                results[i]->add(x, out);
                // wykonujemy krok algorytmu numerycznego
                stepper.do_step(model, x, t, ts);
                // inkrementujemy czas
                t += ts;
            }
            // zatwierdzamy przypadek ucz�cy
            fitness->commitData();
            // obliczamy statystyki
            results[i]->evalStats();
        }
        // wykonujemy ko�cowe obliczenia funkcji przystosowania
        fitness->evaluate();
        // po przerobieniu danych ucz�cych przetwarzamy testowe
        test = true;
    }

    // wykonujemy normalizacj� za pomoc� �redniego odchylenia standardowego dla danych ucz�cych
    for(unsigned i=0; i<mOutputs; ++i) {
        double stdError = stdErrData[i]/rowsData*fitnessData->getSign(i);
        for(unsigned j=0; j<mData.size(); ++j) {
            resultsData[j]->normalize(i, stdError);
        }
        for(unsigned j=0; j<mTest.size(); ++j) {
            resultsTest[j]->normalize(i, stdError);
        }
    }
    // zapisujemy dane
    for(unsigned j=0; j<mData.size(); ++j) {
        resultsData[j]->exec();
    }
    for(unsigned j=0; j<mTest.size(); ++j) {
        resultsTest[j]->exec();
    }
    return fitnessData;
}

void EvalResultsOp::logResults(ResultModel& result, Beagle::GP::System::Handle ioSystem)
{
    mResult = &result;
    test(result.getIndividual(), ioSystem);
}
