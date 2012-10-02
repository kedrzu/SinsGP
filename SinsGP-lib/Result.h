#pragma once
#include "Config.h"
#include "ResultRow.h"
#include "Status.h"

namespace SinsGP {
	//! Klasa reprezentująca wyniki predykcji znalezionego modelu dla jednego przypadku uczącego
	/*!
		Klasa dziedziczy z std::vector<ResultRow>
		Dla celów oceny jakości znalezionego modelu, po zakończeniu procesu uczenia
		dla najlepszych osobników przetwarzane są wszystkie przypadki uczące i testowe
		a wyniki ich zapisywane. Klasa odpowiada za reprezentację danych wynikowych
		dla danego modelu i jednego przypadku uczącego. Odpowiada także za generację
		plików wynikowych CSV i/lub wykresów.
		Dane przechowywane są w postaci wektora obiektów klasSinsGPom::ResultRow.
	*/
	class Result : public std::vector<ResultRow>
	{
	public:

		//! Domyślny kontener obiektów klasy Result
		typedef std::vector<Result*> Bag;

		/*!
			\brief Konstruktor klasy
			\param config Obiekt konfiguracyjny
			\param order Rząd modelu, którego dotyczy obiekt klasy
            \param data Przetwarzany przypadek uczący bądź testo
            \param status Status procesu optymalizacji
			\param test Określa, czy przetwarzane są dane uczące, czy testowe
		*/
        Result(Config& config, int order, Data &data, Status& status, bool test);

		/*!
			\brief Zwraca obiekt konfiguracyjny
			\return Result::mConfig Obiekt konfiguracyjny
		*/
        Config& getConfig() const { return mConfig; }

		/*!
			\brief Zwraca przypadek uczący (lub testowy)
			\return Result::mData Przypadek danych
		*/
        Data& getData() const { return mData; }

		~Result(void);
		/*!
			\brief Tworzenie plików CSV i wykresów z zapisanych danych.

			Dane zachowane w obiekcie zostają wpisane do pliku wynikowego CSV oraz pliku
			CSV do generacji wykresów. Utworzone zostają skrypty Gnuplot i uruchomiony
			program do rysowania wykresów.
		*/
		void exec();


	private:
		Config& mConfig; //!< Obiekt konfiguracyjny

        boost::filesystem::path	mDataFile, //!< Ścieżka do pliku źródłowego
									mCsvPath, //!< Ścieżka do csv
									mPlotPath; //!< Ścieżka do źródła danych do wykresu

        Data& mData; //!< Przetwarzany przypadek uczący bądź testowy

		bool mTest; //!< Czy przetwarzane są dane uczące, czy testowe?
        int mOrder; //!< Rząd modelu
        Status* mStatus; //!< Status procesu optymalizacji
	};
}
