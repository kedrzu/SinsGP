#pragma once
#include "Config.h"
#include "Data.h"

namespace SinsGP {	
	//! Klasa reprezentuje model obiektu dynamicznego z czasem dyskretnym
	class DiscreteModel
	{
	public:
		//! Wektor błędów sygnałów wyjściowych
		typedef std::vector<double> Error;

		/*!
			\brief Konstruktor klasy
			\param config Obiekt konfiguracyjny
			\param individual Przetwarzany model
			\param data Przetwarzany przypadek uczący
			\param context Obiekt reprezentujący aktualny stan procesu uczenia
		*/
        DiscreteModel(Config& config, Beagle::GP::Individual& individual, const Data& data, Beagle::GP::Context& context);
		
		virtual ~DiscreteModel(void);

		/*!
			\brief Implementacja dynamiki obiektu.
			
			Metoda przetwarza przypadek uczący i zwraca w wyniku wektor błędów sygnałów
			wyjściowych w celu obliczenia funkcji przystosowania.
		*/
		virtual Error integrate();
	protected:
		Config& mConfig; //!< Obiekt konfiguracyjny
		const Data& mData; //!< Dane uczące
		Beagle::GP::Context& mContext; //!< Obiekt reprezentujący aktualny stan procesu ucenia
        Beagle::GP::Individual mIndividual; //!< Przetwarzany osobnik (model)
		unsigned mInputs; //!< Liczba wejść
		unsigned mOutputs; //!< Liczba wyjść
		unsigned mOrder; //!< Rząd modelu
        unsigned mMaxOrder; //!< Maksymalny dopuszczalny rząd modelu
        unsigned mObservables; //!< Liczba obserwowalnych zmiennych stanu
        unsigned mDerivatives; //!< Liczba równań stanu będących pochodnymi
        unsigned mStateTrees; //!< Liczba drzew opisująca równania stanu
        unsigned mOutTrees; //!< Liczba drzew opisująca równania wyjść
	};
}
