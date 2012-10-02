#pragma once
#include "FitnessNMSE.h"

namespace SinsGP {
	//! Klasa reprezentuje dane statystyczne dla całego procesu uczenia
	/*!
		W trakcie procesu uczenia otrzymywane są wynikowe modele, które następnie
		zostają przetestowane na danych uczących i testowych, zaś ich wyniki
		statystyczne zostają wypisane w jednym pliku CSV dla lepszego wglądu
		w wyniki optymalizacji genetycznej.
		Statystyki zawierają informacje o wartości przystosowania osobnika dla danych
		uczących i testowych, miary błędów, współczynniki korelacji sygnałów wyjściowych
		i rząd modelu.
		Dane wpisywane są na bieżąco do pliku wynikowego. Klasa została zaprojektowana
		docelowo dla wypisywania danych statystycznych wielowątkowo, oraz tak, aby można
		było uruchomić proces uczenia wielokrotnie, a także w dowolnej chwili zatrzymać
		nie tracąc wyników. W ten sposób otrzymuje się bardzo dużo modeli wynikowych i
		ich porównanie wymaga odpowiednich narzędzi do statystycznej oceny.
		Klasa jest singletonem.
	*/
	class Stats
	{
	public:

        //! Struktura reprezentuje jeden wiersz danych statystycznych
        /*!
            Struktura służy wyłącznie do wstępnego przygotowania wiersza danych
            statystycznych i ich późniejszego dodania do pliku wynikowego przy pomocy
            jednej metody.
        */
        struct Row {
            int run; //!< Numer runa
            int model; //!< Numer modelu
            FitnessNMSE training;
            FitnessNMSE testing;
        };

		/*!
			\brief Konstruktor klasy
			\param config Obiekt konfiguracyjny
		*/
		Stats(Config* config);
		~Stats(void);
		
		/*! 
			\brief Dodanie nowego wiersza statystyk
            \param row Wiersz statystyk
		*/
        void addRow(const Row& row); //< Dodaje wiersz wg danych z bufora
	private:
		Config* mConfig; //!< Obiekt konfiguracyjny
		std::ofstream* mCsv; //!< Stream do utworzenia pliku csv
        boost::filesystem::path mCsvPath; //!< Ścieżka do csv
	};
}
