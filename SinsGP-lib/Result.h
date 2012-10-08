#pragma once
#include "Config.h"
#include "Status.h"
#include "Data.h"
#include <boost/array.hpp>
#include <vector>
#include "SinsGP_Signal.h"

namespace SinsGP {
    //! Klasa reprezentująca wyniki predykcji znalezionego modelu dla jednego przypadku uczącego
    /*!
        Dla celów oceny jakości znalezionego modelu, po zakończeniu procesu uczenia
        dla najlepszych osobników przetwarzane są wszystkie przypadki uczące i testowe
        a wyniki ich zapisywane. Klasa odpowiada za reprezentację danych wynikowych
        dla danego modelu i jednego przypadku uczącego. Odpowiada także za generację
        plików wynikowych CSV i/lub wykresów. Klasa jest tablicą o wielkości 5 złożoną
        z różnych sygnałów:
        - 0 - stan
        - 1 - wyjście z modelu
        - 2 - błąd pomiędzy wyjściem mierzonym a z modelu
        - 3 - wyjście znormalizowane z modelu
        - 4 - błąd pomiędzy wyjściem mierzonym a znormalizowanym z modelu
    */
    class Result
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

        void add(const std::vector<double>& x, const std::vector<double>& y);

        void normalize(unsigned i, double stdErr);
        void normalize(unsigned i, double stdErr, double mean);
        void evalStats();


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
        unsigned mOrder; //!< Rząd modelu
        unsigned mStep;
        Status* mStatus; //!< Status procesu optymalizacji
        Signal::Bag mState;
        Signal::Bag mOutputPredicted;
        Signal::Bag mError;
        Signal::Bag mOutputNormalized;
        Signal::Bag mErrorNormalized;
    };
}
