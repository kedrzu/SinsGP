#pragma once
#include "Config.h"
#include "ContinousModel.h"
#include "Result.h"
#include "ContinousModel.h"

namespace SinsGP { 
	//! Klasa reprezentuje obserwator stanu obiektu
	/*!
		W bibliotece Boost.odeint jedynym sposobem na dostanie si� do warto�ci stanu
		obiektu w trakcie oblicze�, czy te� po nich jest tzw obserwator. Jest to obiekt
		klasy z operatorem () b�d� funkcja, kt�ra jest wywo�ywana po ka�dym kroku
		algorytmu numerycznego.

		Opr�cz zapisywania wynik�w oblicze�, kontrolowana jest r�wnie� stabilno�� uk�adu.
		Uk�ad niestabilny jest du�ym problemem dla algorytmu numerycznego i powoduje
		bardzo du�e spowolnienie b�d� nawet zatrzymanie oblicze�. Konieczna jest wi�c
		bie��ca kontrola stabilno�ci uk�adu, kt�ra dokonywana jest na 2 sposoby:
		sprawdzanie �redniego b��du znormalizowanego (je�eli przekroczy on pewn�
		dopuszczaln� warto��, to uk�ad uznaje si� za niestabilny) oraz kontrola �redniej
		liczby krok�w algorytmu numerycznego na jednostk� czasu (je�eli algorytm z
		adaptacyjnie dobieranym krokiem czasowym wykonuje ich bardzo wiele, to
		prawdopodobnie nie mo�e znale�� prawid�owego rozwi�zania, ergo uk�ad jest niestabilny).
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
			\param x Stan uk�adu
			\param t Aktualny czas
			\throw ContinousModel::Exception W razie wykrycia niestabilno�ci
			
			Argumentami jest stan i czas (tylko do odczytu). W metodzie nast�puje 
			aktualizacja bufora stanu oraz sprawdzanie stabilno�ci uk�adu.
		*/
        void operator() (const ContinousModel::StateType& x, const double t);
    protected:
        ContinousModel::StateType& mX; //!< Bufor do zapisu aktualnego stanu
		Config& mConfig; //!< Obiekt konfiguracyjny
		int mOutputs, //!< Liczba wyj�� modelu
			mOrder, //!< Rz�d modelu
			mSteps; //!< Liczba krok�w algorytmu numerycznego
    };
}
