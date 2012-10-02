#pragma once
#include "Status.h"
#include "Config.h"

namespace SinsGP {

	
	//! Klasa evolvera Open Beagle. 
	/*!
		Evolver w Open Beagle jest obiektem odpowiadającym za zastosowanie algorytmu
		genetycznego do zbioru osobników (vivarium).
		Kod metod Evolver::initialize() jest praktycznie w całości skopiowany z klasy
		bazowej. Jedyna różnica polega na zmianie niektórych parametrów algorytmu (konkretnie
		ilości drzew osobnika) uczącego przed inicjalizacją tzw. operatorów. Jest to
		konieczne, a nie ma takiej możliwości poza ciałem klasy Beagle::Evolver.
	 */
	class Evolver :	public Beagle::GP::Evolver
	{
	public:

		//! Alokator danych (wymagany przez Open Beagle)
		typedef Beagle::AllocatorT<SinsGP::Evolver,Beagle::GP::Evolver::Alloc> Alloc;
		//! Inteligentny uchwyt (wymagany przez Open Beagle)
		typedef Beagle::PointerT<SinsGP::Evolver,Beagle::GP::Evolver::Handle> Handle;
		//! Domyślny kontener (wymagany przez Open Beagle)
		typedef Beagle::ContainerT<SinsGP::Evolver,Beagle::GP::Evolver::Bag> Bag;

		//! Domyślny konstruktor (konieczy przy użyciu alokatorów)
		Evolver();

		/*!
			\brief Konstruktor klasy
			\param inEvalOp Uchwyt do obiektu reprezentującego funkcję przystosowania
			\param config Obiekt konfiguracyjny
		*/
        explicit Evolver(Beagle::EvaluationOp::Handle inEvalOp, Config* config, Status& status);
		virtual ~Evolver();

		/*!
			\brief Inicjalizacja evolvera przy użyciu pliku konfiguracyjnego Beagle.
			\param ioSystem Uchwyt do obiektu klasy Beagle::System
			\param inConfigFilename Ścieżka do pliku konfiguracyjnego Open Beagle
		*/
		virtual void initialize(Beagle::System::Handle ioSystem, Beagle::string inConfigFilename);
		
		/*!
			\brief Inicjalizacja evolvera przy użyciu pliku konfiguracyjnego Beagle.
			\param ioSystem Uchwyt do obiektu klasy Beagle::System
			\param ioArgc Liczba argumentów podana przy uruchomieniu programu z konsoli
			\param ioArgv Wartości argumentów podane przy uruchomieniu programu z konsoli
		*/
		virtual void initialize(Beagle::System::Handle ioSystem, int& ioArgc, char** ioArgv);

	protected:
		Config* mConfig; //!< Obiekt konfiguracyjny
        Status* mStatus; //!< Status procesu optymalizacji

		//! Ustawia odpowiednią liczbę drzew każdego osobnika, aby zgadzała się z szukanym modelem.
        void initSinsGP(Beagle::System::Handle ioSystem);
	};

}
