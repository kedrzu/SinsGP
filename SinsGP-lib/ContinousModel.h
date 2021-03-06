﻿#ifndef SINSGP_CONTINOUSMODEL_H
#define SINSGP_CONTINOUSMODEL_H

#include "Config.h"
#include "Data.h"

namespace SinsGP {
    //! Klasa reprezentuje model obiektu dynamicznego z czasem ciągłym
    /*!
        Biblioteka numeryczna Boost.odeint wymaga zaimplementowania klasy reprezentującej
        obiekt dynamiczny. Cała dynamika obiektu zawarta jest w metodzie operator(), która
        reprezentuje układ równań różniczkowych I rzędu.
    */
    class ContinousModel
    {
    public:

        //! Reprezentacja wektora stanu.
        typedef std::vector<double> StateType;
        /*!
            \brief Konstruktor klasy
            \param config Obiekt konfiguracyjny
            \param individual Przetwarzany model
            \param data Przetwarzany przypadek uczący
            \param context Obiekt reprezentujący obecny stan algorytmu genetycznego
        */
        ContinousModel(Config& config, Beagle::GP::Individual& individual, const Data& data, Beagle::GP::Context& context);
        ~ContinousModel(void);
        /*!
            \brief Implementacja układu równań różniczkowych modelu
            \param x Obecny stan układu
            \param dxdt Pochodna stanu, która jest obliczana w metodzie
            \param t Obecny czas

            Obecny stan i czas są argumentami tej metody. Pochodna dxdt jest wartością
            wyjściową, którą należy obliczyć. W celu obliczenia pochodnych stanu należy
            odczytać wartości sygnałów wejściowych zapisane w przetwarzanym przypadku
            uczącym. Ponieważ są one zapisane w postaci ciągu liczb, to należy dokonać
            aproksymacji funkcją ciągłą: prostokątami w przód, bądź trapezami, w zależności
            od ustawień.

            Na przetwarzany wektor x składa się wektor stanu układu oraz wektor całki
            z wartości bezwzględnej błędu znormalizowanego, który służy do obliczenia
            funkcji przystosowania, a więc jakości modelu. Na podstawie stanu oraz wejść
            obliczane jest wyjście układu, porównywane z sygnałami wyjściowymi zapisanymi
            w przypadku uczącym (także aproksymowane).
        */
        void operator()(const StateType &x, StateType &dxdt, const double t);

        void setInputs(unsigned step);
        std::vector<double> getOutput(StateType &x);

    protected:
        Config& mConfig; //!< Obiekt konfiguracyjny
        Beagle::GP::Individual& mIndividual; //!< Przetwarzany model
        const Data& mData; //!< Przetwarzany przypadek uczący
        Beagle::GP::Context& mContext; //!< Obiekt reprezentujący aktualny stan procesu uczenia
        unsigned mInputs; //!< Liczba wejść modelu
        unsigned mOutputs; //!< Liczba wyjść modelu
        unsigned mOrder; //!< Rząd modelu
        unsigned mMaxOrder; //!< Maksymalny dopuszczalny rząd modelu
        unsigned mObservables; //!< Liczba obserwowalnych zmiennych stanu
        unsigned mDerivatives; //!< Liczba równań stanu będących pochodnymi
        unsigned mStateTrees; //!< Liczba drzew opisująca równania stanu
        unsigned mOutTrees; //!< Liczba drzew opisująca równania wyjść
    };
}




#endif // SINSGP_ContinousModel_H
