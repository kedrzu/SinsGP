#include "stdafx.h"
#include "Evolver.h"

using namespace Beagle;
using namespace SinsGP;

SinsGP::Evolver::Evolver(void) : GP::Evolver()
{
}

SinsGP::Evolver::Evolver(Beagle::EvaluationOp::Handle inEvalOp, Config* config, Status &status)
    : GP::Evolver(inEvalOp), mConfig(config), mStatus(&status)
{
}

SinsGP::Evolver::~Evolver(void)
{
}

/*!
 *  \brief Initialize the evolver, its operators and the system.
 *  \param ioSystem Handle to the system of the evolution.
 *  \param ioArgc Number of elements on the command-line.
 *  \param ioArgv Element on the command-line.
 */
void SinsGP::Evolver::initialize(System::Handle ioSystem, int& ioArgc, char** ioArgv)
{
  Beagle_StackTraceBeginM();
  // Get system handle.
  mSystemHandle = ioSystem;

  // Parse command-line.
  parseCommandLine(*ioSystem, ioArgc, ioArgv);

  // Logging message.
  Beagle_LogDetailedM(
    ioSystem->getLogger(),
    "evolver", "Beagle::Evolver",
    "Initializing evolver"
  );

  // Add configuration dumper parameter.
  if(ioSystem->getRegister().isRegistered("ec.conf.dump")) {
    mConfigDumper =
      castHandleT<ConfigurationDumper>(ioSystem->getRegister().getEntry("ec.conf.dump"));
  }
  else {
    mConfigDumper = new ConfigurationDumper(*ioSystem, *this, "");
	Beagle::string lLongDescripDump = "Filename used to dump the configuration. ";
    lLongDescripDump += "A configuration dump means that a configuration file is ";
    lLongDescripDump += "written with the evolver (including the composing operators) ";
    lLongDescripDump += "and the register (including the registered parameters and their ";
    lLongDescripDump += "default values). No evolution is conducted on a configuration dump. ";
    lLongDescripDump += "An empty string means no dump.";
    Register::Description lFileDumperDescription(
      "Configuration dump filename",
      "String",
      "\"\"",
      lLongDescripDump
    );
    ioSystem->getRegister().addEntry("ec.conf.dump", mConfigDumper, lFileDumperDescription);
  }

  // Add configuration file name.
  if(ioSystem->getRegister().isRegistered("ec.conf.file")) {
    mFileName = castHandleT<String>(ioSystem->getRegister().getEntry("ec.conf.file"));
  }
  else {
    mFileName = new String("");
	Beagle::string lLongDescripFN = "The name of a configuration file containing ";
    lLongDescripFN += "evolver and parameter values. A typical configuration file can ";
    lLongDescripFN += "be created with parameter \"ec.conf.dump\".";
    Register::Description lFileNameDescription(
      "Configuration filename",
      "String",
      "\"\"",
      lLongDescripFN
    );
    ioSystem->getRegister().addEntry("ec.conf.file", mFileName, lFileNameDescription);
  }

  // Add population size parameter
  if(ioSystem->getRegister().isRegistered("ec.pop.size")) {
    mPopSize = castHandleT<UIntArray>(ioSystem->getRegister().getEntry("ec.pop.size"));
  } else {
    mPopSize = new UIntArray(1,100);
	Beagle::string lLongDescrip("Number of demes and size of each deme of the population. ");
    lLongDescrip += "The format of an UIntArray is S1,S2,...,Sn, where Si is the ith value. ";
    lLongDescrip += "The size of the UIntArray is the number of demes present in the ";
    lLongDescrip += "vivarium, while each value of the vector is the size of the corresponding ";
    lLongDescrip += "deme.";
    Register::Description lDescription(
      "Vivarium and demes sizes",
      "UIntArray",
      "100",
      lLongDescrip
    );
    ioSystem->getRegister().addEntry("ec.pop.size", mPopSize, lDescription);
  }

  // Initialize operators.
  initializeOperators(*ioSystem);

  // Initialize the system.
  ioSystem->initialize(ioArgc, ioArgv);

  initSinsGP(ioSystem);

  // Calling system post-initialization hook.
  ioSystem->postInit();

  // Calling post-initialization hook of operators.
  postInitOperators(*ioSystem);

  Beagle_StackTraceEndM("void Evolver::initialize(System::Handle, int&, char**)");
}

/*!
 *  \brief Initialize the evolver, its operators and the system.
 *  \param ioSystem Handle to the system of the evolution.
 *  \param inConfigFilename Filename containing configuration values.
 */
void SinsGP::Evolver::initialize(System::Handle ioSystem, Beagle::string inConfigFilename)
{
  Beagle_StackTraceBeginM();
  // Get system handle.
  mSystemHandle = ioSystem;

  // Reading evolver configuration, if any.
  if(inConfigFilename.empty() == false) readEvolverFile(inConfigFilename);

  // Logging message.
  Beagle_LogDetailedM(
    ioSystem->getLogger(),
    "evolver", "Beagle::Evolver",
    "Initializing evolver"
  );

  // Add configuration dumper parameter.
  if(ioSystem->getRegister().isRegistered("ec.conf.dump")) {
    mConfigDumper =
      castHandleT<ConfigurationDumper>(ioSystem->getRegister().getEntry("ec.conf.dump"));
  }
  else {
    mConfigDumper = new ConfigurationDumper(*ioSystem, *this, "");
	Beagle::string lLongDescripDump = "Filename used to dump the configuration. ";
    lLongDescripDump += "A configuration dump means that a configuration file is ";
    lLongDescripDump += "written with the evolver (including the composing operators) ";
    lLongDescripDump += "and the register (including the registered parameters and their ";
    lLongDescripDump += "default values). No evolution is conducted on a configuration dump. ";
    lLongDescripDump += "An empty string means no dump.";
    Register::Description lFileDumperDescription(
      "Configuration dump filename",
      "String",
      "\"\"",
      lLongDescripDump
    );
    ioSystem->getRegister().addEntry("ec.conf.dump", mConfigDumper, lFileDumperDescription);
  }

  // Add configuration file name.
  if(ioSystem->getRegister().isRegistered("ec.conf.file")) {
    mFileName = castHandleT<String>(ioSystem->getRegister().getEntry("ec.conf.file"));
  }
  else {
    mFileName = new String(inConfigFilename);
    Beagle::string lDefaultFileName = Beagle::string("\"") + inConfigFilename + Beagle::string("\"");
    Beagle::string lLongDescripFN = "The name of a configuration file containing ";
    lLongDescripFN += "evolver and parameter values. A typical configuration file can ";
    lLongDescripFN += "be created with parameter \"ec.conf.dump\".";
    Register::Description lFileNameDescription(
      "Configuration filename",
      "String",
      lDefaultFileName,
      lLongDescripFN
    );
    ioSystem->getRegister().addEntry("ec.conf.file", mFileName, lFileNameDescription);
  }

  // Add population size parameter
  if(ioSystem->getRegister().isRegistered("ec.pop.size")) {
    mPopSize = castHandleT<UIntArray>(ioSystem->getRegister().getEntry("ec.pop.size"));
  } else {
    mPopSize = new UIntArray(1,100);
    Beagle::string lLongDescrip("Number of demes and size of each deme of the population. ");
    lLongDescrip += "The format of an UIntArray is S1,S2,...,Sn, where Si is the ith value. ";
    lLongDescrip += "The size of the UIntArray is the number of demes present in the ";
    lLongDescrip += "vivarium, while each value of the vector is the size of the corresponding ";
    lLongDescrip += "deme.";
    Register::Description lDescription(
      "Vivarium and demes sizes",
      "UIntArray",
      "100",
      lLongDescrip
    );
    ioSystem->getRegister().addEntry("ec.pop.size", mPopSize, lDescription);
  }

  initSinsGP(ioSystem);

  // Initialize operators.
  initializeOperators(*ioSystem);

  // Initialize the system.
  ioSystem->initialize(inConfigFilename);

  initSinsGP(ioSystem);

  // Calling system post-initialization hook.
  ioSystem->postInit();

  // Calling post-initialization hook of operators.
  postInitOperators(*ioSystem);

  Beagle_StackTraceEndM("void Evolver::initialize(System::Handle, string)");
}

void SinsGP::Evolver::initSinsGP(Beagle::System::Handle ioSystem) {
	// KONFIGURACJA ZMIENNYCH WYJŒCIOWYCH I STANU
	//
	// ka¿dej zmiennej wyjœciowej i zmiennej stanu odpowiada jedno drzewo osobnika
	// nale¿y ustawiæ w rejestrze liczbê maksymaln¹ i minimaln¹, jak¹ bedzie mia³ osobnik
	// oczywiœcie min=max, tak aby ka¿dy osobnik mia³ tyle samo drzew

	// zmienne wyjœciowe
	// okreœlenie minimalnej iloœci drzew w jednym osobniku
    unsigned min = mConfig->getOutputs().size()+mConfig->getMinOrder()-mConfig->getObservables().size()-mConfig->getDerivatives().size();
    unsigned max = mConfig->getOutputs().size()+mConfig->getMaxOrder()-mConfig->getObservables().size()-mConfig->getDerivatives().size();
	if(ioSystem->getRegister().isRegistered("gp.init.mintree")) {
        UInt::Handle entry = new UInt(min);
		ioSystem->getRegister().modifyEntry("gp.init.mintree", entry);
	} else {		
        UInt::Handle entry = new UInt(min);
		ioSystem->getRegister().addEntry("gp.init.mintree", entry, Register::Description());
	}
	// okreœlenie maksymalnej iloœci drzew w jednym osobniku
	if(ioSystem->getRegister().isRegistered("gp.init.maxtree")) {
        UInt::Handle entry = new UInt(max);
		ioSystem->getRegister().modifyEntry("gp.init.maxtree", entry);
	} else {		
        UInt::Handle entry = new UInt(max);
		ioSystem->getRegister().addEntry("gp.init.maxtree", entry, Register::Description());
	}

    // KONFIGURACJA ŒCIE¯KI ZAPISU MILESTONE
    if(ioSystem->getRegister().isRegistered("ms.write.prefix")) {
        String::Handle ms = castHandleT<String>(ioSystem->getRegister().getEntry("ms.write.prefix"));
        std::string pref = mStatus->getModelPath().string();
        String::Handle entry = new String (pref + ms->getWrappedValue());
        ioSystem->getRegister().modifyEntry("ms.write.prefix", entry);
    }
}
