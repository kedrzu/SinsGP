#include "stdafx.h"
#include "SinsGP_Signal.h"

using namespace SinsGP;

Signal::Signal()
{
}

Signal::Stats::Stats()
    : mean(0), rms(0), stdErr(0)
{
}

Signal::Signal(unsigned size)
{
    Beagle_StackTraceBeginM();
    this->reserve(size);
    Beagle_StackTraceEndM("void SinsGP::Signal::Signal(unsigned size)");
}

void Signal::evalSum()
{
    Beagle_StackTraceBeginM();
    mSum = 0;
    for(unsigned i=0; i<this->size(); ++i) {
        mSum += (*this)[i];
    }
    Beagle_StackTraceEndM("void SinsGP::Signal::evalSum()");
}

void Signal::evalMean()
{
    Beagle_StackTraceBeginM();
    mStats.mean = mSum / this->size();
    Beagle_StackTraceEndM("void SinsGP::Signal::evalMean()");
}

void Signal::evalRMS()
{
    Beagle_StackTraceBeginM();
    double pow2sum = 0;
    for(unsigned i=0; i<this->size(); ++i) {
        pow2sum += (*this)[i]*(*this)[i];
    }
    mStats.rms = sqrt(pow2sum / this->size());
    Beagle_StackTraceEndM("void SinsGP::Signal::evalRMS()");
}

void Signal::evalStdErr() {
    Beagle_StackTraceBeginM();
    double variance = 0;
    for(unsigned i=0; i<this->size(); ++i) {
        double sub = (*this)[i] - mStats.mean;
        variance += sub*sub;
    }
    mStats.stdErr = sqrt(variance);
    Beagle_StackTraceEndM("void SinsGP::Signal::evalStdErr()");
}

const Signal::Stats& Signal::evalStats()
{
    Beagle_StackTraceBeginM();
    evalMean();
    evalRMS();
    evalStdErr();
    return mStats;
    Beagle_StackTraceEndM("void SinsGP::Signal::evalStats()");
}

void Signal::push_back(double value)
{
    Beagle_StackTraceBeginM();
    std::vector<double>::push_back(value);
    mSum += value;
    Beagle_StackTraceEndM("void SinsGP::Signal::push_back(double value)");
}

double Signal::corelation(const Signal& secondSignal) const
{
    Beagle_StackTraceBeginM();
    Beagle_AssertM(this->size() == secondSignal.size());
    double meanFirst = mSum / this->size();
    double meanSecond = secondSignal.getSum() / secondSignal.size();
    double covariance = 0;
    double varianceFirst = 0;
    double varianceSecond = 0;
    for(unsigned i=0; i<this->size(); ++i) {
        double first = (*this)[i] - meanFirst;
        double second = secondSignal[i] - meanSecond;
        covariance += first*second;
        varianceFirst += first*first;
        varianceSecond += second*second;
    }
    return covariance / (sqrt(varianceFirst)*sqrt(varianceSecond));
    Beagle_StackTraceEndM("double SinsGP::Signal::correlation(const Signal& secondSignal) const ");
}

void Signal::normalizeThis(double stdErr) {
    Beagle_StackTraceBeginM();
    Beagle_AssertM(mStats.stdErr > 0);
    for(unsigned i=0; i<this->size(); ++i) {
        (*this)[i] = (*this)[i] / mStats.stdErr * stdErr;
    }
    Beagle_StackTraceEndM("void SinsGP::Signal::normalizeThis(double stdErr)");
}

void Signal::normalizeThis(double stdErr, double mean) {
    Beagle_StackTraceBeginM();
    Beagle_AssertM(mStats.stdErr > 0);
    for(unsigned i=0; i<this->size(); ++i) {
        (*this)[i] = ((*this)[i] - mStats.mean) / mStats.stdErr * stdErr + mean;
    }
    Beagle_StackTraceEndM("void SinsGP::Signal::normalizeThis(double stdErr, double mean)");
}

Signal::Handle Signal::normalize(double stdErr) const {
    Beagle_StackTraceBeginM();
    Beagle_AssertM(mStats.stdErr > 0);
    Signal::Handle result = new Signal(this->size());
    for(unsigned i=0; i<this->size(); ++i) {
        result->push_back((*this)[i] / mStats.stdErr * stdErr);
    }
    return result;
    Beagle_StackTraceEndM("void SinsGP::Signal::normalize(double stdErr)");
}

Signal::Handle Signal::normalize(double stdErr, double mean) const {
    Beagle_StackTraceBeginM();
    Beagle_AssertM(mStats.stdErr > 0);
    Signal::Handle result = new Signal(this->size());
    for(unsigned i=0; i<this->size(); ++i) {
        result->push_back(((*this)[i] - mStats.mean) / mStats.stdErr * stdErr + mean);
    }
    return result;
    Beagle_StackTraceEndM("void SinsGP::Signal::normalize(double stdErr, double mean)");
}

Signal::Handle Signal::operator +(const Signal& second) const {
    Beagle_StackTraceBeginM();
    Beagle_AssertM(this->size() == second.size());
    Signal::Handle result = new Signal(this->size());
    for(unsigned i=0; i<this->size(); ++i) {
        result->push_back((*this)[i] + second[i]);
    }
    return result;
    Beagle_StackTraceEndM("Signal* SinsGP::Signal::operator +(const Signal& second) const");
}

Signal::Handle Signal::operator -(const Signal& second) const {
    Beagle_StackTraceBeginM();
    Beagle_AssertM(this->size() == second.size());
    Signal::Handle result = new Signal(this->size());
    for(unsigned i=0; i<this->size(); ++i) {
        result->push_back((*this)[i] - second[i]);
    }
    return result;
    Beagle_StackTraceEndM("Signal* SinsGP::Signal::operator -(const Signal& second) const");
}

Signal::Handle Signal::operator +(const Signal::Handle second) const {
    Beagle_StackTraceBeginM();
    Beagle_AssertM(this->size() == second->size());
    Signal::Handle result = new Signal(this->size());
    for(unsigned i=0; i<this->size(); ++i) {
        result->push_back((*this)[i] + (*second)[i]);
    }
    return result;
    Beagle_StackTraceEndM("Signal* SinsGP::Signal::operator +(const Signal& second) const");
}

Signal::Handle Signal::operator -(const Signal::Handle second) const {
    Beagle_StackTraceBeginM();
    Beagle_AssertM(this->size() == second->size());
    Signal::Handle result = new Signal(this->size());
    for(unsigned i=0; i<this->size(); ++i) {
        result->push_back((*this)[i] - (*second)[i]);
    }
    return result;
    Beagle_StackTraceEndM("Signal* SinsGP::Signal::operator -(const Signal& second) const");
}
