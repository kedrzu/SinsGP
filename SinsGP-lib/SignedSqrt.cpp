#include "stdafx.h"
#include "SignedSqrt.h"
#include <cmath>

using namespace Beagle;
using namespace SinsGP;

SignedSqrt::SignedSqrt() : GP::Primitive(1, "SSQRT")
{
}

void SignedSqrt::execute(GP::Datum& outResult,GP::Context& ioContext) {
    Double arg;
    get1stArgument(arg,ioContext);
    Double& result = castObjectT<Double&>(outResult);
    if(arg > 0) {
        result = sqrt(arg);
    } else {
        result = -sqrt(-arg);
    }
}
