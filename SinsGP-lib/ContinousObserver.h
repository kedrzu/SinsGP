#pragma once
#include "Config.h"
#include "ContinousModel.h"
#include "Result.h"
#include "ContinousModel.h"

namespace SinsGP { 
	//! Klasa reprezentuje obserwator stanu obiektu
	/*!
		W bibliotece Boost.odeint jedynym sposobem na dostanie siê do wartoœci stanu
		obiektu w trakcie obliczeñ, czy te¿ po nich jest tzw obserwator. Jest to obiekt
		klasy z operatorem () b¹dŸ funkcja, która jest wywo³ywana po ka¿dym kroku
		algorytmu numerycznego.

		Oprócz zapisywania wyników obliczeñ, kontrolowana jest równie¿ stabilnoœæ uk³adu.
		Uk³ad niestabilny jest du¿ym problemem dla algorytmu numerycznego i powoduje
		bardzo du¿e spowolnienie b¹dŸ nawet zatrzymanie obliczeñ. Konieczna jest wiêc
		bie¿¹ca kontrola stabilnoœci uk³adu, która dokonywana jest na 2 sposoby:
		sprawdzanie œredniego b³êdu znormalizowanego (je¿eli przekroczy on pewn¹
		dopuszczaln¹ wartoœæ, to uk³ad uznaje siê za niestabilny) oraz kontrola œredniej
		liczby kroków algorytmu numerycznego na jednostkê czasu (je¿eli algorytm z
		adaptacyjnie dobieranym krokiem czasowym wykonuje ich bardzo wiele, to
		prawdopodobnie nie mo¿e znaleŸæ prawid³owego rozwi¹zania, ergo uk³ad jest niestabilny).
	*/
    class ContinousObserver
	{
	public:
		/*!
			\brief Konstruktor klasy
			\param config Obiekt konfiguracyjny
			\param x Bufor do zapisu stanu
		*/
		ContinousObserver(Config& config, ContinousModel::StateType& x);
        ~ContinousObserver(void) {}
		/*!
			\brief Implementacja operacji obserwacji.
			\param x Stan uk³adu
			\param t Aktualny czas
			\throw ContinousModel::Exception W razie wykrycia niestabilnoœci
			
			Argumentami jest stan i czas (tylko do odczytu). W metodzie nastêpuje 
			aktualizacja bufora stanu oraz sprawdzanie stabilnoœci uk³adu.
		*/
        void operator() (const ContinousModel::StateType& x, const double t);
    protected:
        ContinousModel::StateType& mX; //!< Bufor do zapisu aktualnego stanu
		Config& mConfig; //!< Obiekt konfiguracyjny
		int mOutputs, //!< Liczba wyjœæ modelu
			mOrder, //!< Rz¹d modelu
			mSteps; //!< Liczba kroków algorytmu numerycznego
    };
}
