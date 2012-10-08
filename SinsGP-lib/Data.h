#pragma once
#include "Config.h"
#include "SinsGP_Signal.h"
#include "stdafx.h"

namespace SinsGP {
//! Klasa reprezentuje jeden przypadek danych (uczących bądź testowych).
class Data
{
public:

    //! Domyślny kontener obiektów klasy Data
    typedef std::vector<Data*> Bag;

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
        \brief Zwraca czas trwania przypadku uczącego
        \return Data::mDuration Czas trwania przypadku uczącego
    */
    double getDuration() const { return mDuration; }
    /*!
        \brief Zwraca liczbę wierszy danych
        \return unsigned mRows
    */
    unsigned getRows() const { return mRows; }
    const Signal& getInputs(unsigned i) const { return mInputs[i]; }
    const std::vector<Signal>& getInputs() const { return mInputs; }
    const Signal& getOutputs(unsigned i) const { return mOutputs[i]; }
    const std::vector<Signal>& getOutputs() const { return mOutputs; }

private:
    Config& mConfig; //!< Obiekt konfiguracyjny
    boost::filesystem::path mFile; //!< Nazwa wczytanego pliku
    std::vector<Signal> mInputs; //!< Sygnały wejściowe
    std::vector<Signal> mOutputs; //!< Sygnały wyjściowe
    unsigned mRows; //!< Liczba wierszy
    double mDuration; //!< Długość czasu trwania przypadku uczącego
};
}
