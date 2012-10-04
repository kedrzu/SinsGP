#ifndef EPHEMERALDOUBLEWIDE_H
#define EPHEMERALDOUBLEWIDE_H

#include "stdafx.h"

namespace SinsGP {
class EphemeralDoubleWide : public Beagle::GP::EphemeralT<Beagle::Double> {

public:

  //! GP::EphemeralDouble allocator type.
  typedef Beagle::AllocatorT<EphemeralDoubleWide,Beagle::GP::EphemeralT<Beagle::Double>::Alloc>
          Alloc;
  //! GP::EphemeralDouble handle type.
  typedef Beagle::PointerT<EphemeralDoubleWide,Beagle::GP::EphemeralT<Beagle::Double>::Handle>
          Handle;
  //! GP::EphemeralDouble bag type.
  typedef Beagle::ContainerT<EphemeralDoubleWide,Beagle::GP::EphemeralT<Beagle::Double>::Bag>
          Bag;

  explicit EphemeralDoubleWide(Beagle::Double::Handle inValue=NULL, Beagle::string inName="EW");
  virtual ~EphemeralDoubleWide() { }

  virtual Beagle::GP::Primitive::Handle generate(std::string inName, Beagle::GP::Context& ioContext);

};
}

#endif // EPHEMERALDOUBLEWIDE_H
