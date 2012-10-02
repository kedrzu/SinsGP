#ifndef SINSGP_CONTINOUSMODELCONSTSTEP_H
#define SINSGP_CONTINOUSMODELCONSTSTEP_H

#include "Config.h"
#include "Data.h"

namespace SinsGP {
    //! Klasa reprezentuje model obiektu dynamicznego z czasem ci�g�ym
    /*!
        Biblioteka numeryczna Boost.odeint wymaga zaimplementowania klasy reprezentuj�cej
        obiekt dynamiczny. Ca�a dynamika obiektu zawarta jest w metodzie operator(), kt�ra
        reprezentuje uk�ad r�wna� r�niczkowych I rz�du.
    */
    class ContinousModelConstStep
    {
    public:

        //! Reprezentacja wektora stanu.
        typedef std::vector<double> StateType;
        /*!
            \brief Konstruktor klasy
            \param config Obiekt konfiguracyjny
            \param individual Przetwarzany model
            \param data Przetwarzany przypadek ucz�cy
            \param context Obiekt reprezentuj�cy obecny stan algorytmu genetycznego
        */

        ContinousModelConstStep(Config& config, Beagle::GP::Individual& individual, const Data& data, Beagle::GP::Context& context);
        ~ContinousModelConstStep(void);
        /*!
            \brief Implementacja uk�adu r�wna� r�niczkowych modelu
            \param x Obecny stan uk�adu
            \param dxdt Pochodna stanu, kt�ra jest obliczana w metodzie
            \param t Obecny czas

            Obecny stan i czas s� argumentami tej metody. Pochodna dxdt jest warto�ci�
            wyj�ciow�, kt�r� nale�y obliczy�. W celu obliczenia pochodnych stanu nale�y
            odczyta� warto�ci sygna��w wej�ciowych zapisane w przetwarzanym przypadku
            ucz�cym. Poniewa� s� one zapisane w postaci ci�gu liczb, to nale�y dokona�
            aproksymacji funkcj� ci�g��: prostok�tami w prz�d, b�d� trapezami, w zale�no�ci
            od ustawie�.

            Na przetwarzany wektor x sk�ada si� wektor stanu uk�adu oraz wektor ca�ki
            z warto�ci bezwzgl�dnej b��du znormalizowanego, kt�ry s�u�y do obliczenia
            funkcji przystosowania, a wi�c jako�ci modelu. Na podstawie stanu oraz wej��
            obliczane jest wyj�cie uk�adu, por�wnywane z sygna�ami wyj�ciowymi zapisanymi
            w przypadku ucz�cym (tak�e aproksymowane).
        */
        void operator()(const StateType &x, StateType &dxdt, const double t);
        void nextStep() { ++mStep; }
        unsigned getStep() { return mStep; }

    protected:
        Config& mConfig; //!< Obiekt konfiguracyjny
        const Data& mData; //!< Przetwarzany przypadek ucz�cy
        Beagle::GP::Context& mContext; //!< Obiekt reprezentuj�cy aktualny stan procesu uczenia
        Beagle::GP::Individual& mIndividual; //!< Przetwarzany model
        unsigned mInputs; //!< Liczba wej�� modelu
        unsigned mOutputs; //!< Liczba wyj�� modelu
        unsigned mOrder; //!< Rz�d modelu
        unsigned mMaxOrder; //!< Maksymalny dopuszczalny rz�d modelu
        unsigned mObservables; //!< Liczba obserwowalnych zmiennych stanu
        unsigned mDerivatives; //!< Liczba r�wna� stanu b�d�cych pochodnymi
        unsigned mStateTrees; //!< Liczba drzew opisuj�ca r�wnania stanu
        unsigned mOutTrees; //!< Liczba drzew opisuj�ca r�wnania wyj��
        unsigned mStep; //!< Licznik aktualnego wiersza przypadku ucz�cego
    };
}


#endif // SINSGP_CONTINOUSMODELCONSTSTEP_H
