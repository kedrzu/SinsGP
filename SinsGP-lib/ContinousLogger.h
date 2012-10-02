#pragma once
#include "ContinousModel.h"
#include "Result.h"

namespace SinsGP {
	//! Klasa reprezentuje obserwer obiektu z jednoczesnym zapisywaniem wyników do obiektu klasySinsGPm::Result
	/*!
        Klasa jest zasadniczo modelem, który zapisuje na bieżąco wyniki obliczeń
		do obiektu klasy SinsGP::Result, aby później zapisać je do plików CSV bądź
        wygenerować wykresy.
	*/
    class ContinousLogger : public SinsGP::ContinousModel
	{
	public:
          /*!
            \brief Konstruktor klasy
            \param config Obiekt konfiguracyjny
            \param individual Przetwarzany model
            \param data Przetwarzany przypadek uczący
            \param context Obiekt reprezentujący obecny stan algorytmu genetycznego
        */
        ContinousLogger(Config& config, Beagle::GP::Individual& individual, const Data& data, Beagle::GP::Context& context);

        ~ContinousLogger(void) {}
        /*!
            \brief Implementacja układu równań różniczkowych modelu
            \param x Obecny stan układu
            \param dxdt Pochodna stanu, która jest obliczana w metodzie
            \param t Obecny czas
        */
        void operator()(const StateType &x, StateType &dxdt, const double t);

        /*!
         *  \brief Zwraca wiersz danych
         *  \return mResultRow wiersz danych
         */
        const ResultRow& getResultRow() const { return mResultRow; }
    protected:
        ResultRow mResultRow; //!< Wiersz wyniku danych
	};
}
