#ifndef EVALRESULTSOP_H
#define EVALRESULTSOP_H

#pragma once
#include "Config.h"
#include "Data.h"
#include "ResultModel.h"
#include "Stats.h"

namespace SinsGP {

    class EvalResultsOp : public Beagle::GP::EvaluationOp
    {
    public:

        // TYPEDEFS
        //! Alokator danych (wymagany przez Open Beagle)
        typedef Beagle::AllocatorT<EvalResultsOp,Beagle::GP::EvaluationOp::Alloc> Alloc;
        //! Inteligenty uchwyt (wymagany przez Open Beagle)
        typedef Beagle::PointerT<EvalResultsOp,Beagle::GP::EvaluationOp::Handle> Handle;
        //! Domy�lny kontener danych (wymagany przez Open Beagle)
        typedef Beagle::ContainerT<EvalResultsOp,Beagle::GP::EvaluationOp::Bag> Bag;

        /*!
            \brief Konstruktor klasy
            \param config Obiekt konfiguracyjny
            \param data Zbi�r danych ucz�cych.
            \param test Zbi�r danych testowych.
            \param stats Obiekt do zapisu statystycznych wynik�w uczenia.
         */
        explicit EvalResultsOp(Config& config, Data::Bag data, Data::Bag test, Stats* stats);
        ~EvalResultsOp(void);


        /*!
         * \brief Implementacja funkcji przystosowania.
         *
         * Metoda reprezentuje w�a�ciw� funkcj� przystosowania. Oblicza przystosowanie
         * podanego osobnika i zwraca w postaci obiektu Beagle::Fitness. Implementuje
         * dynamik� modelu reprezentowanego przez osobnika, a tak�e zawiera kod s�u��cy
         * do generowania wynik�w uczenia w momencie zako�czenia dzia�ania algorytmu genetycznego.
         *
         * Funkcja przystosowania liczona jest z wzoru 1/(1+err), gdzie err jest wyliczonym b��dem,
         * b�d�cym sum� b��d�w znormalizowanych po wszystkich przypadkach ucz�cych. Normalizacj�
         * b��du dokonuje si� przez jego podzielenie przez warto�� skuteczn� danego sygna�u wyj�ciowego
         * w celu unikni�cia zdominowania procesu uczenia przez jedno z wyj��.
         *
         * \param inIndividual Osobnik, kt�rego funkcja przystosowania jest obliczana
         * \param ioContext Obiekt Open Beagle reprezentuj�cy aktualny stan algorytmu genetycznego
         */
        virtual Beagle::Fitness::Handle evaluate(Beagle::GP::Individual& inIndividual,
                                        Beagle::GP::Context& ioContext);

        /*!
            \brief Zapis osobnika do plik�w wynikowych.

            Metoda uruchamiana jest po sko�czonym procesie uczenia na zbiorze najlepszych osobnik�w.
            Odpowiada za zapis wynik�w uczenia oraz testowania osobnika. Dokonuje tego poprzez
            uruchomienie metody DynObjEvalOp::evaluate() na osobniku z jednocze�nie ustawionymi
            odpowiednimi flagami w postaci zmiennych prywatnych.

            Model kt�ry ma zosta� poddany ocenie podawany jest w postaci obiektu ResultModel.
            Wyniki uczenia zapisywane s� w tym obiekcie.

            \param result Wynikowy model, kt�ry ma zosta� przetworzony
            \param ioSystem Obiekt Open Beagle reprezentuj�cy ustawienia algorytmu genetycznego
         */
        void logResults(ResultModel& result, Beagle::GP::System::Handle ioSystem);


    private:
        Config& mConfig; //!< Obiekt konfiguracyjny.
        Data::Bag mData; //!< Dane ucz�ce
        Data::Bag mTest; //!< Dane testowe

        unsigned mInputs, //!< Liczba zmiennych wej�ciowych modelu.
                 mOutputs, //!< Liczba zmiennych wyj�ciowych modelu.
                 mMaxOrder; //!< Maksymalny rz�d obiektu.

        ResultModel* mResult; //!< Obiekt do zapisu wynik�w uczenia
        Stats* mStats; //!< Log do zapisu statystycznych wynik�w uczenia
    };
}


#endif // EVALRESULTSOP_H
