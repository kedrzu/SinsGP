#pragma once
#include "Config.h"
#include "DiscreteModel.h"
#include "Result.h"

namespace SinsGP {
	//! Klasa reprezentuje obiekt dynamiczny z czasem dyskretnym z jednoczesnym zapisywaniem wyników
	/*!
		Klasa ma takie samo działanie, jakSinsGPm::DiscreteModel, z tym że jednocześnie
		wyniki zapisywane są do obiektuSinsGPm::Result w celu późniejszego zapisania
		do plików csv bądź rysowania wykresów.
	*/
	class DiscreteLogger : public DiscreteModel
	{
	public:
		/*!
			\brief Konstruktor klasy
			\param individual Przetwarzany model
			\param result Obiekt wyników (zawiera także obiekt przypadku ucząceSinsGPdom::Data i obiekt konfiguracyjSinsGPdom::Config)
			\param context Obiekt reprezentujący aktualny stan procesu uczenia
		*/
        DiscreteLogger(Beagle::GP::Individual& individual, Result& result, Beagle::GP::Context& context);
		
		~DiscreteLogger(void);

		/*!
			\brief Implementacja dynamiki obiektu.
			
			Metoda przetwarza przypadek uczący i zwraca w wyniku wektor błędów sygnałów
			wyjściowych w celu obliczenia funkcji przystosowania.
		*/
		Error integrate();
	protected:
		Result& mResult;
	};
}
