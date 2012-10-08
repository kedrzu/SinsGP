#include "stdafx.h"
#include <fstream>
#include "gnuplotpp.h"
#include "Result.h"

using namespace SinsGP;
using namespace boost::filesystem;
using namespace gnuplotpp::detail;

Result::Result(Config& config, int order, Data& data, Status &status, bool test)
    : mConfig(config),
      mDataFile(data.getFile().filename()),
      mData(data),
      mTest(test),
      mOrder(order),
      mStep(0),
      mStatus(&status),
      mState(new Signal::Alloc, order),
      mOutputPredicted(new Signal::Alloc, config.getOutputs().size()),
      mError(new Signal::Alloc, config.getOutputs().size()),
      mOutputNormalized(new Signal::Alloc, config.getOutputs().size()),
      mErrorNormalized(new Signal::Alloc, config.getOutputs().size())
{
    for(unsigned i=0; i<mOrder; ++i) {
        mState[i] = new Signal(mData.getRows());
    }
    for(unsigned i=0; i<config.getOutputs().size(); ++i) {
        mOutputPredicted[i] = new Signal(mData.getRows());
        mError[i] = new Signal(mData.getRows());
    }
}

Result::~Result(void)
{ 
}

void Result::add(const vector<double> &x, const vector<double> &y)
{
    Beagle_StackTraceBeginM();
    Beagle_AssertM(x.size() == mOrder);
    Beagle_AssertM(y.size() == mConfig.getOutputs().size());
    for(unsigned i=0; i<x.size(); ++i) {
        mState[i]->push_back(x[i]);
    }
    for(unsigned i=0; i<y.size(); ++i) {
        mOutputPredicted[i]->push_back(y[i]);
        mError[i]->push_back(mData.getOutputs(i)[mStep]-y[i]);
    }
    mStep++;
    Beagle_StackTraceEndM("void SinsGP::Result::add(const vector &x, const vector &y)");
}

void Result::normalize(unsigned i, double stdErr)
{
    Beagle_StackTraceBeginM();
    mOutputNormalized[i] = mOutputPredicted[i]->normalize(stdErr);
    mOutputNormalized[i]->evalStats();
    mErrorNormalized[i] = mData.getOutputs(i) - mOutputNormalized[i];
    mErrorNormalized[i]->evalStats();
    Beagle_StackTraceEndM("void SinsGP::Result::normalize(unsigned i, double stdErr)");
}

void Result::normalize(unsigned i, double stdErr, double mean)
{
    Beagle_StackTraceBeginM();
    mOutputNormalized[i] = mOutputPredicted[i]->normalize(stdErr, mean);
    mOutputNormalized[i]->evalStats();
    mErrorNormalized[i] = mData.getOutputs(i) - mOutputNormalized[i];
    mErrorNormalized[i]->evalStats();
    Beagle_StackTraceEndM("void SinsGP::Result::normalize(unsigned i, double stdErr, double mean)");
}

void Result::evalStats() {
    Beagle_StackTraceBeginM();
    for(unsigned i=0; i<mOrder; ++i) {
        mState[i]->evalStats();
    }
    for(unsigned i=0; i<mConfig.getOutputs().size(); ++i) {
        mOutputPredicted[i]->evalStats();
        mError[i]->evalStats();
    }
    Beagle_StackTraceEndM("void SinsGP::Result::evalStats()");
}

void Result::exec() 
{
    Beagle_StackTraceBeginM();
    int flags = mTest ? mConfig.getResultIndTest() : mConfig.getResultIndData();
    path run = mConfig.getPaths().run / mStatus->getModelPath();
    path temp = mConfig.getPaths().temp / mStatus->getModelPath();
    create_directories(run);
    create_directories(temp);

    // utworzenie pliku csv
    if(flags & Config::CSV) {
        mCsvPath = run / path((mTest ? "test-" : "data-") + mDataFile.filename().string() + ".csv");

        // brak danych do zapisu
        if(mStep == 0)
            throw runtime_error("SinsGP::Result::exec() Brak danych do zapisania w pliku" + mCsvPath.string());

        // stream
        ofstream csv(mCsvPath.string().c_str());

        if(!csv.good())
            throw std::runtime_error("SinsGP::Result::Result() Nie udalo sie otworzyc do zapisu pliku " + mCsvPath.string());

        // nagłówki pliku
        csv << "time";
        for(unsigned i=0; i<mOrder; ++i) {
            csv << "\t" << "x" << (i+1);
        }
        for(unsigned i=0; i<mConfig.getOutputs().size(); i++) {
            csv << "\t" << "y" << (i+1) << "-measured";
            csv << "\t" << "y" << (i+1) << "-predicted";
            csv << "\t" << "y" << (i+1) << "-error";
            csv << "\t" << "y" << (i+1) << "-normalized";
            csv << "\t" << "y" << (i+1) << "-error-norm";
        }

        // zapisujemy do pliku
        for(unsigned i=0; i<mStep; i++) {
            // czas
            csv << endl << (i*mConfig.getSamplingTime());
            // stan
            for(unsigned j=0; j<mOrder; ++j) {
                csv << "\t" << (*mState[j])[i];
            }
            //wyjścia
            for(unsigned j=0; j<mConfig.getOutputs().size(); j++) {
                csv << "\t" << mData.getOutputs(j)[i];
                csv << "\t" << (*mOutputPredicted[j])[i];
                csv << "\t" << (*mError[j])[i];
                csv << "\t" << (*mOutputNormalized[j])[i];
                csv << "\t" << (*mErrorNormalized[j])[i];
            }
        }
        csv.close();
    }
    if(flags & Config::PLOT) {
        mPlotPath = temp / path((mTest ? "test-" : "data-") + mDataFile.filename().string() + "-plot.csv");

        // brak danych do zapisu
        if(mStep == 0)
            throw runtime_error("SinsGP::Result::exec() Brak danych do zapisania w pliku" + mCsvPath.string());

        // stream
        ofstream csv(mPlotPath.string().c_str());

        if(!csv.good())
            throw std::runtime_error("SinsGP::Result::Result() Nie udalo sie otworzyc do zapisu pliku z danymi do wykresu " + mPlotPath.string());

        // zapisujemy do pliku
        for(unsigned i=0; i<mStep; i++) {
            // czas
            csv << endl << (i*mConfig.getSamplingTime());
            //wyjścia
            for(unsigned j=0; j<mConfig.getOutputs().size(); j++) {
                csv << "\t" << mData.getOutputs(j)[i];
                csv << "\t" << (*mOutputNormalized[j])[i];
                csv << "\t" << (*mErrorNormalized[j])[i];
            }
        }
        csv.close();

        for( unsigned i=0; i<mConfig.getOutputs().size(); i++ ){
            singleplot g;

            // cerr << g.set().str() << endl;
            // exit(1);

            std::string title = "\"Run: " + Beagle::int2str(mStatus->getRun()) + " Model: " + Beagle::int2str(mStatus->getModel()) + " File: " + (mTest ? "test" : "data") + "/" + mDataFile.string() + " Variable: y" + Beagle::int2str(i+1) + "\"";
            path plotfile = run / path(mDataFile.string() + "-y" + Beagle::int2str(i) + "-plot.png");
            g.set() << gpset( "term", "png size 1000,500" ) << endl
                    << gpset("grid")
                    << gpset("title", title) << endl
                    << gpset("output", "'" + plotfile.string() + "'" ) << endl;

            g.x.range(0, mStep*mConfig.getSamplingTime());

            g.plotfile(mPlotPath.string(), 1, i*3+2) << " t 'measured'" << " with l lw 2.5 lc rgb '#33ccff'";
            g.plotfile(mPlotPath.string(), 1, i*3+3) << " t 'predicted'" << " with l lw 1 lc rgb '#000099'";
            g.plotfile(mPlotPath.string(), 1, i*3+4) << " t 'error'" << " with l lw 1 lc rgb '#ff0000'";

            path gpFile = temp / path((mTest ? "test-" : "data-") + mDataFile.string() + "-y" + Beagle::int2str(i+1) + ".gp");
            g.save( gpFile.string());
            string cmd = "gnuplot \"" + gpFile.string() + "\"";
            system(cmd.c_str());
        }
    }
    Beagle_StackTraceEndM("void SinsGP::Result::exec()");
}
