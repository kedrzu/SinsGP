#ifndef SINSGP_FITNESS_H
#define SINSGP_FITNESS_H

#include "stdafx.h"
#include "Data.h"

namespace SinsGP {

template<class T>
class Fitness : public Beagle::Fitness, protected std::vector<T>
{
public:

    typedef Beagle::AbstractAllocT<SinsGP::Fitness<T>,Beagle::Fitness::Alloc> Alloc;
    typedef Beagle::PointerT<SinsGP::Fitness<T>,Beagle::Fitness::Handle> Handle;
    typedef Beagle::ContainerT<SinsGP::Fitness<T>,Beagle::Fitness::Bag> Bag;

    Fitness(unsigned outputs)
        : std::vector<T>(outputs)  { }
    Fitness() {}
    void setData(const Data& data) { mData = &data; postSetData();}
    void setData(const Data* data) { mData = data; postSetData();}
    virtual ~Fitness() { }
    virtual void add(const std::vector<double>& output) = 0;
    virtual void commitData() = 0;
    virtual void evaluate() = 0;
    const T& operator[](unsigned i) const { return std::vector<T>::operator[](i); }
    T& operator[](unsigned i) { return std::vector<T>::operator[](i); }

protected:
    virtual void postSetData() {}
    const Data* mData;
};
}



#endif // FITNESS_H
