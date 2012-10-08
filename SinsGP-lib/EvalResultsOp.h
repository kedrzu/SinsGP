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
        //! Domyœlny kontener danych (wymagany przez Open Beagle)
        typedef Beagle::ContainerT<EvalResultsOp,Beagle::GP::EvaluationOp::Bag> Bag;

        /*!
            \brief Konstruktor klasy
            \param config Obiekt konfiguracyjny
            \param data Zbiór danych ucz¹cych.
            \param test Zbiór danych testowych.
            \param stats Obiekt do zapisu statystycznych wyników uczenia.
         */
        explicit EvalResultsOp(Config& config, Data::Bag data, Data::Bag test, Stats* stats);
        ~EvalResultsOp(void);


        /*!
         * \brief Implementacja funkcji przystosowania.
         *
         * Metoda reprezentuje w³aœciw¹ funkcjê przystosowania. Oblicza przystosowanie
         * podanego osobnika i zwraca w postaci obiektu Beagle::Fitness. Implementuje
         * dynamikê modelu reprezentowanego przez osobnika, a tak¿e zawiera kod s³u¿¹cy
         * do generowania wyników uczenia w momencie zakoñczenia dzia³ania algorytmu genetycznego.
         *
         * Funkcja przystosowania liczona jest z wzoru 1/(1+err), gdzie err jest wyliczonym b³êdem,
         * bêd¹cym sum¹ b³êdów znormalizowanych po wszystkich przypadkach ucz¹cych. Normalizacjê
         * b³êdu dokonuje siê przez jego podzielenie przez wartoœæ skuteczn¹ danego sygna³u wyjœciowego
         * w celu unikniêcia zdominowania procesu uczenia przez jedno z wyjœæ.
         *
         * \param inIndividual Osobnik, którego funkcja przystosowania jest obliczana
         * \param ioContext Obiekt Open Beagle reprezentuj¹cy aktualny stan algorytmu genetycznego
         */
        virtual Beagle::Fitness::Handle evaluate(Beagle::GP::Individual& inIndividual,
                                        Beagle::GP::Context& ioContext);

        /*!
            \brief Zapis osobnika do plików wynikowych.

            Metoda uruchamiana jest po skoñczonym procesie uczenia na zbiorze najlepszych osobników.
            Odpowiada za zapis wyników uczenia oraz testowania osobnika. Dokonuje tego poprzez
            uruchomienie metody DynObjEvalOp::evaluate() na osobniku z jednoczeœnie ustawionymi
            odpowiednimi flagami w postaci zmiennych prywatnych.

            Model który ma zostaæ poddany ocenie podawany jest w postaci obiektu ResultModel.
            Wyniki uczenia zapisywane s¹ w tym obiekcie.

            \param result Wynikowy model, który ma zostaæ przetworzony
            \param ioSystem Obiekt Open Beagle reprezentuj¹cy ustawienia algorytmu genetycznego
         */
        void logResults(ResultModel& result, Beagle::GP::System::Handle ioSystem);


    private:
        Config& mConfig; //!< Obiekt konfiguracyjny.
        Data::Bag mData; //!< Dane ucz¹ce
        Data::Bag mTest; //!< Dane testowe

        unsigned mInputs, //!< Liczba zmiennych wejœciowych modelu.
                 mOutputs, //!< Liczba zmiennych wyjœciowych modelu.
                 mMaxOrder; //!< Maksymalny rz¹d obiektu.

        ResultModel* mResult; //!< Obiekt do zapisu wyników uczenia
        Stats* mStats; //!< Log do zapisu statystycznych wyników uczenia
    };
}


#endif // EVALRESULTSOP_H
