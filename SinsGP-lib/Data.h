#pragma once
#include "Config.h"
#include "DataRow.h"

namespace SinsGP {
	//! Klasa reprezentuje jeden przypadek danych (uczących bądź testowych).
	/*!
		Klasa dziedziczy z std::vector<DataRow>.
		Każdy obiekt klasy reprezentuje jeden przypadek danych uczących bądź
		testowych. Jest to więc jeden przebieg czasowy z zarejestrowanymi
		sygnałami wejściowymi oraz wyjściowymi obiektu dynamicznego. Każdy z przypadków
		uczących zapisany jest w postaci pliku CSV.
		Klasa odpowiada za parsowanie danych z pliku CSV i ich reprezentację.
		Dane reprezentowane są przez wektor obiektów typSinsGPom::DataRow, a więc
		pojedynczych wierszy danych.
		Klasa ponadto oblicza parametry statystyczne danych	do celów normalizacji błędu
		(moc skuteczna dla każdej zmiennej wyjściowej) i statystycznej oceny 
		jakości uczenia.
	*/
	class Data : public std::vector<DataRow>
	{
	public:

		//! Domyślny kontener obiektów klasy Data
		typedef std::vector<Data*> Bag;

		//! Reprezentuje parametry statystyczne jednego sygnału
		/*!
			Struktura zawiera parametry statystyczne sygnału dla celów różnych obliczeń
			takich jak normalizacja błędu uczenia, czy obliczenia współczynników korelacji
			do oceny jakości modelu.
		*/
		struct Stats {
			double RMS; //!< Wartość skuteczna sygnału.
			double integralSquare; //!< Całka z kwadratu sygnału
			double integral; //!< Całka z sygnału
			double avg; //!< Wartość średnia
			double std; //!< Odchylenie standardowe
            Stats() : RMS(0), integralSquare(0), integral(0), avg(0), std(0) {} //! Konstruktor domyślny
		};

		//! Konstruktor parsujący plik CSV
		/*!
			W konstruktorze następuje odczytanie danych z podanego pliku CSV i ich walidacja.
			W razie niepowodzenia wczytania pliku wyrzucony zostaje wyjątek std::exception o
			odpowiedniej treści.
			
			\param config Obiekt konfiguracyjny.
			\param file Ścieżka do pliku CSV.
			\throw std::runtime_error W razie niepowodzenia wczytania pliku.
		*/
		Data(SinsGP::Config& config, boost::filesystem::path file);
		
		virtual ~Data(void);

		/*!
			\brief Zwraca obiekt konfiguracyjny
			\return Data::mConfig Obiekt konfiguracyjny
		*/
        const SinsGP::Config& getConfig() const { return mConfig; }
		/*!
			\brief Zwraca ścieżkę do pliku źródłowego
			\return Data::mFile Ścieżka do pliku źródłowego
		*/
        boost::filesystem::path getFile() const { return mFile; }
		/*!
			\brief Zwraca parametry statystyczne sygnałów wyjściowych
			\return Data::mOutStats Parametry statystyczne sygnałów wyjściowych
		*/
        const std::vector<Data::Stats>& getOutStats() const { return mOutStats; }
		/*!
			\brief Zwraca parametry statystyczne jednego synału wyjściowego
			\param i Numer wyjścja
			\return Data::mOutStats[i] Parametry statystyczne sygnału wyjściowego
		*/
        const Data::Stats& getOutStats(int i) const { return mOutStats[i]; }
		/*!
			\brief Zwraca czas trwania przypadku uczącego
			\return Data::mDuration Czas trwania przypadku uczącego
		*/
        double getDuration() const { return mDuration; }
		

	private:
		// ZMIENNE
        Config& mConfig; //!< Obiekt konfiguracyjny
        double mDuration; //!< Długość czasu trwania przypadku uczącego
		boost::filesystem::path mFile; //!< Nazwa wczytanego pliku
        std::vector<Data::Stats> mOutStats; //!< Statystyki sygnałów wyjściowych
        //int mRows; //!< Liczba wierszy
	};
}
