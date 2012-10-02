#ifndef SignedSqrt_H
#define SignedSqrt_H


namespace SinsGP {

class SignedSqrt : public Beagle::GP::Primitive
{
public:
    SignedSqrt();
    virtual void execute(Beagle::GP::Datum &outResult, Beagle::GP::Context &ioContext);
};

}

#endif // SignedSqrt_H
