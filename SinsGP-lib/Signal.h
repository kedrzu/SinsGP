#ifndef SINSGP_SIGNAL_H
#define SINSGP_SIGNAL_H

#include <vector>
#include <cmath>
#include <boost/math/special_functions/fpclassify.hpp>

namespace SinsGP {

class Signal : protected std::vector<double>
{
public:
    Signal()
        : mSum(0), mMean(0), mPow2Sum(0), mRMS(0)
    {
    }

    Signal(unsigned size)
        : mSum(0), mMean(0), mPow2Sum(0), mRMS(0)
    {
        this->reserve(size);
    }

    double operator[](unsigned i) const { return std::vector<double>::operator[](i); }

    double evalMean()
    {
        mMean = mSum / this->size();
        return mMean;
    }

    double evalRMS()
    {
            mPow2Sum = 0;
            for(int i=0; i<this->size(); ++i) {
                mPow2Sum += (*this)[i]*(*this)[i];
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
        std::vector<double>::push_back(value);
        mSum += value;
    }

    double getMean() const { return mMean; }

    double getRMS() const { return mRMS; }

    double getSum() const { return mSum; }

    double correlation(const Signal& secondSignal) const {
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
        double corr = covariance / (sqrt(varianceFirst)*sqrt(varianceSecond));

        // je¿eli któryœ sygna³ jest to¿samoœciowo równy 0
        // w równaniu wyst¹pi dzielenie przez 0 i zwrócona zostanie wartoœæ NaN
        // nale¿y wówczas zwróciæ korelacjê równ¹ -2
        // dziêki temu algorytm unikaæ bêdzie modeli z równaniami to¿samoœciowo
        // równymi zeru
        if(boost::math::isnan<double>(corr)) return -2;
        else return corr;
    }

protected:
    double mSum;
    double mMean;
    double mPow2Sum;
    double mRMS;
};

}

#endif // SINSGP_SIGNAL_H
