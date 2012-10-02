#pragma once
#include "ContinousModel.h"
#include "Result.h"

namespace SinsGP {
	//! Klasa reprezentuje obserwer obiektu z jednoczesnym zapisywaniem wyników do obiektu klasySinsGPm::Result
	/*!
		Klasa jest zasadniczo obserwerem, a więc ma podobne założenia co
		SinsGP::ContinousObserver. Ponadto zapisuje na bieżąco wyniki obliczeń
		do obiektu klasy SinsGP::Result, aby później zapisać je do plików CSV bądź
		wygenerować wykresy. Z uwagi na implementację biblioteki Boost.odeint
		w metodzie obserwatora przekazywany jest wyłącznie stan, a więc nie można
		przekazać żadnych innych parametrów (konkretnie sygnałów wyjściowych modelu, 
		które nie są całkowane). Potrzeba więc niezależnie od obiektu modelu, w identyczny
		sposób odtworzyć wartości tych sygnałów. Z uwagi na to implementacja klasy
		musi być podobna do ContinousModel. Ponieważ jednak obie te klasy mają inne
		funkcje, to wykorzystane jest dziedziczenie chronione.
	*/
	class ContinousLogger : protected SinsGP::ContinousModel
	{
	public:
		/*!
			\brief Konstruktor klasy
			\param individual Przetwarzany model
			\param result Obiekt do zapisu wyników.
			\param context Obiekt reprezentujący aktualny stan procesu uczenia
            \param x Obiekt do którego zapisywany jest stan
		*/
        ContinousLogger(Beagle::GP::Individual &individual, Result& result, ContinousModel::StateType& x, Beagle::GP::Context &context);
        ~ContinousLogger(void) {}
		/*!
			\brief Implementacja operacji obserwacji.
			\param x Stan układu
			\param t Aktualny czas
			\throw ContinousModel::Exception W razie wykrycia niestabilności
			
			Argumentami jest stan i czas (tylko do odczytu). W metodzie następuje 
			sprawdzanie stabilności układu oraz dopisanie wiersza danych do obiektu
			wynikowego.
		*/
		void operator() (const StateType& x, const double t);
	protected:
        ContinousModel::StateType& mX; //!< Bufor do zapisu aktualnego stanu
		Result& mResult; //!< Obiekt wyniku modelu
	};
}
