#ifndef SINSGP_CONTINOUSMODELCONSTSTEP_H
#define SINSGP_CONTINOUSMODELCONSTSTEP_H

#include "Config.h"
#include "Data.h"

namespace SinsGP {
    //! Klasa reprezentuje model obiektu dynamicznego z czasem ci¹g³ym
    /*!
        Biblioteka numeryczna Boost.odeint wymaga zaimplementowania klasy reprezentuj¹cej
        obiekt dynamiczny. Ca³a dynamika obiektu zawarta jest w metodzie operator(), która
        reprezentuje uk³ad równañ ró¿niczkowych I rzêdu.
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
            \param data Przetwarzany przypadek ucz¹cy
            \param context Obiekt reprezentuj¹cy obecny stan algorytmu genetycznego
        */

        ContinousModelConstStep(Config& config, Beagle::GP::Individual& individual, const Data& data, Beagle::GP::Context& context);
        ~ContinousModelConstStep(void);
        /*!
            \brief Implementacja uk³adu równañ ró¿niczkowych modelu
            \param x Obecny stan uk³adu
            \param dxdt Pochodna stanu, która jest obliczana w metodzie
            \param t Obecny czas

            Obecny stan i czas s¹ argumentami tej metody. Pochodna dxdt jest wartoœci¹
            wyjœciow¹, któr¹ nale¿y obliczyæ. W celu obliczenia pochodnych stanu nale¿y
            odczytaæ wartoœci sygna³ów wejœciowych zapisane w przetwarzanym przypadku
            ucz¹cym. Poniewa¿ s¹ one zapisane w postaci ci¹gu liczb, to nale¿y dokonaæ
            aproksymacji funkcj¹ ci¹g³¹: prostok¹tami w przód, b¹dŸ trapezami, w zale¿noœci
            od ustawieñ.

            Na przetwarzany wektor x sk³ada siê wektor stanu uk³adu oraz wektor ca³ki
            z wartoœci bezwzglêdnej b³êdu znormalizowanego, który s³u¿y do obliczenia
            funkcji przystosowania, a wiêc jakoœci modelu. Na podstawie stanu oraz wejœæ
            obliczane jest wyjœcie uk³adu, porównywane z sygna³ami wyjœciowymi zapisanymi
            w przypadku ucz¹cym (tak¿e aproksymowane).
        */
        void operator()(const StateType &x, StateType &dxdt, const double t);
        void nextStep() { ++mStep; }
        unsigned getStep() { return mStep; }

    protected:
        Config& mConfig; //!< Obiekt konfiguracyjny
        const Data& mData; //!< Przetwarzany przypadek ucz¹cy
        Beagle::GP::Context& mContext; //!< Obiekt reprezentuj¹cy aktualny stan procesu uczenia
        Beagle::GP::Individual& mIndividual; //!< Przetwarzany model
        unsigned mInputs; //!< Liczba wejœæ modelu
        unsigned mOutputs; //!< Liczba wyjœæ modelu
        unsigned mOrder; //!< Rz¹d modelu
        unsigned mMaxOrder; //!< Maksymalny dopuszczalny rz¹d modelu
        unsigned mObservables; //!< Liczba obserwowalnych zmiennych stanu
        unsigned mDerivatives; //!< Liczba równañ stanu bêd¹cych pochodnymi
        unsigned mStateTrees; //!< Liczba drzew opisuj¹ca równania stanu
        unsigned mOutTrees; //!< Liczba drzew opisuj¹ca równania wyjœæ
        unsigned mStep; //!< Licznik aktualnego wiersza przypadku ucz¹cego
    };
}


#endif // SINSGP_CONTINOUSMODELCONSTSTEP_H
