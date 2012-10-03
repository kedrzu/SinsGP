#ifndef SINSGP_SIGNAL_H
#define SINSGP_SIGNAL_H

#include <vector>
#include <cmath>

namespace SinsGP {

template<bool sum=true, bool pow2sum=false, bool stats=false>
class Signal : protected std::vector<double>
{
public:
    //template<bool sum, bool pow2sum, bool stats>
    Signal()
        : mSum(0), mMean(0), mPow2Sum(0), mRMS(0)
    {
    }

    Signal(unsigned size)
        : mSum(0), mMean(0), mPow2Sum(0), mRMS(0)
    {
        this->reserve(size);
    }

    double operator[](unsigned i) const { return (*this)[i]; }

    double evalMean()
    {
        if(!sum && !stats) {
            mSum = 0;
            for(int i=0; i<this->size(); ++i) {
                mSum += (*this)[i];
            }
        }
        mMean = mSum / this->size();
        return mMean;
    }

    double evalRMS()
    {
        if(!pow2sum && !stats) {
            mPow2Sum = 0;
            for(int i=0; i<this->size(); ++i) {
                mPow2Sum += (*this)[i]*(*this)[i];
            }
        }
        mRMS = sqrt(mPow2Sum / this->size());
        return mRMS;
    }

    void evalStats()
    {
        evalMean();
        evalRMS();
    }

    void push_back(double value)
    {
        this->push_back(value);
        if(sum || stats) mSum += value;
        if(pow2sum || stats) mPow2Sum += value*value;
        if(stats) evalStats();
    }

    double getMean() const { return mMean; }

    double getRMS() const { return mRMS; }

    double getSum() const { return mSum; }

    double correlation(const Signal& secondSignal) {
        double covariance = 0;
        double varianceFirst = 0;
        double varianceSecond = 0;
        for(int i=0; i<this->size(); ++i) {
            double first = (*this)[i] - mMean;
            double second = secondSignal[i] - secondSignal.mMean;
            covariance += first*second;
            varianceFirst += first*first;
            varianceSecond += second*second;
        }
        return covariance / (sqrt(varianceFirst)*sqrt(varianceSecond));
    }

protected:
    double mSum;
    double mMean;
    double mPow2Sum;
    double mRMS;
};

}

#endif // SINSGP_SIGNAL_H
