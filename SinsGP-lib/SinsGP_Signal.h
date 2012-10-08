#ifndef SINSGP_SIGNAL_H
#define SINSGP_SIGNAL_H

#include <vector>
#include <beagle/GP.hpp>

namespace SinsGP {

class Signal : public Beagle::Object, public std::vector<double>
{
public:

    // TYPEDEFS
    //! Alokator danych (wymagany przez Open Beagle)
    typedef Beagle::AllocatorT<Signal,Beagle::Object::Alloc> Alloc;
    //! Inteligenty uchwyt (wymagany przez Open Beagle)
    typedef Beagle::PointerT<Signal,Beagle::Object::Handle> Handle;
    //! Domyœlny kontener danych (wymagany przez Open Beagle)
    typedef Beagle::ContainerT<Signal,Beagle::Object::Bag> Bag;

    struct Stats {
        double mean;
        double rms;
        double stdErr;
        Stats();
    };

    Signal();

    Signal(unsigned size);

    void evalSum();

    void evalMean();

    void evalRMS();

    void evalStdErr();

    const Signal::Stats& evalStats();

    void push_back(double value);

    const Signal::Stats& getStats() const { return mStats; }

    double corelation(const Signal& secondSignal) const;

    void normalizeThis(double stdErr);

    void normalizeThis(double stdErr, double mean);

    Signal::Handle normalize(double stdErr) const;

    Signal::Handle normalize(double stdErr, double mean) const;

    Signal::Handle operator +(const Signal& second) const;

    Signal::Handle operator -(const Signal& second) const;

    Signal::Handle operator +(const Signal::Handle second) const;

    Signal::Handle operator -(const Signal::Handle second) const;

    void resize(unsigned size) { std::vector<double>::resize(size); }

    double getSum() const { return mSum; }

protected:
    Signal::Stats mStats;
    double mSum;
};

}

#endif // SINSGP_SIGNAL_H
