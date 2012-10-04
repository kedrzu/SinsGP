#include "stdafx.h"
#include "EphemeralDoubleWide.h"
#include <cmath>

using namespace SinsGP;
using namespace Beagle;
using namespace std;


EphemeralDoubleWide::EphemeralDoubleWide(Double::Handle inValue, Beagle::string inName) :
  GP::EphemeralT<Double>(inValue, inName)
{ }


/*!
 *  \brief  Generate a new random ephemeral Double constant between [-1,1].
 *  \param  inName Name of the constant.
 *  \param  ioContext Context to use to generate the value.
 *  \return Handle to the ephemeral Double constant generated.
 */
GP::Primitive::Handle EphemeralDoubleWide::generate(Beagle::string inName, GP::Context& ioContext)
{
  Beagle_StackTraceBeginM();
  double factor = ioContext.getSystem().getRandomizer().rollUniform(-1.,1.);
  double exp = ioContext.getSystem().getRandomizer().rollUniform(-4.,3.);
  Double::Handle lValue = new Double(factor*pow(10, exp));
  return new EphemeralDoubleWide(lValue, inName);
  Beagle_StackTraceEndM("GP::Primitive::Handle SinsGP::EphemeralDoubleWide::generate(string inName, GP::Context& ioContext)");
}
