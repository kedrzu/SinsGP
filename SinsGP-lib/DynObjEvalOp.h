#pragma once
#include "Config.h"
#include "Data.h"
#include "ResultModel.h"
#include "Stats.h"

namespace SinsGP {


	//! Klasa reprezentuje funkcję przystosowania algorytmu genetycznego.
	/*!
	 *	Jest to zasadnicza część algorytmu genetycznego, którą należy zdefiniować w Open Beagle.
	 *	Odpowiada za implementację funkcji przystosowania, która używana jest przez algorytm
	 *	genetyczny do oceny osobników i przeprowadzania operacji genetycznych.
	 *
	 *	Tak jak wszystkie obiekty używane w Open Beagle, klasa musi zdefiniować swój alokator,
	 *	inteligentny uchwyt i domyślny kontener. Wynika to z modelu danych przyjętego w bibliotece
	 *	Open Beagle.
	 *
	 *	Najważniejszą metodą klasy jest DynObjEvalOp::evaluate(), jako że jest to zasadnicza
	 *	implementacja przystosowania osobnika.
	 */
	class DynObjEvalOp : public Beagle::GP::EvaluationOp
	{
	public:

		// TYPEDEFS
		//! Alokator danych (wymagany przez Open Beagle)
		typedef Beagle::AllocatorT<DynObjEvalOp,Beagle::GP::EvaluationOp::Alloc> Alloc;
		//! Inteligenty uchwyt (wymagany przez Open Beagle)
		typedef Beagle::PointerT<DynObjEvalOp,Beagle::GP::EvaluationOp::Handle> Handle;
		//! Domyślny kontener danych (wymagany przez Open Beagle)
		typedef Beagle::ContainerT<DynObjEvalOp,Beagle::GP::EvaluationOp::Bag> Bag;

		/*!
			\brief Konstruktor klasy
			\param config Obiekt konfiguracyjny
            \param data Zbiór danych uczących.
		 */
        explicit DynObjEvalOp(Config& config, Data::Bag data);
		~DynObjEvalOp(void);


		/*!
		 * \brief Implementacja funkcji przystosowania.
		 *
		 * Metoda reprezentuje właściwą funkcję przystosowania. Oblicza przystosowanie
		 * podanego osobnika i zwraca w postaci obiektu Beagle::Fitness. Implementuje
         * dynamikę modelu reprezentowanego przez osobnika, a także zawiera kod służący
		 * do generowania wyników uczenia w momencie zakończenia działania algorytmu genetycznego.
		 *
		 * Funkcja przystosowania liczona jest z wzoru 1/(1+err), gdzie err jest wyliczonym błędem,
		 * będącym sumą błędów znormalizowanych po wszystkich przypadkach uczących. Normalizację
		 * błędu dokonuje się przez jego podzielenie przez wartość skuteczną danego sygnału wyjściowego
		 * w celu uniknięcia zdominowania procesu uczenia przez jedno z wyjść.
		 * 
		 * \param inIndividual Osobnik, którego funkcja przystosowania jest obliczana
		 * \param ioContext Obiekt Open Beagle reprezentujący aktualny stan algorytmu genetycznego
		 */
		virtual Beagle::Fitness::Handle evaluate(Beagle::GP::Individual& inIndividual,
										Beagle::GP::Context& ioContext);

		/*!
			\brief Zapis osobnika do plików wynikowych.
			
			Metoda uruchamiana jest po skończonym procesie uczenia na zbiorze najlepszych osobników.
			Odpowiada za zapis wyników uczenia oraz testowania osobnika. Dokonuje tego poprzez
			uruchomienie metody DynObjEvalOp::evaluate() na osobniku z jednocześnie ustawionymi
			odpowiednimi flagami w postaci zmiennych prywatnych.

			Model który ma zostać poddany ocenie podawany jest w postaci obiektu ResultModel.
			Wyniki uczenia zapisywane są w tym obiekcie.

			\param result Wynikowy model, który ma zostać przetworzony
			\param ioSystem Obiekt Open Beagle reprezentujący ustawienia algorytmu genetycznego
		 */
        void logResults(ResultModel& result, Beagle::GP::System::Handle ioSystem);


	private:
        Config& mConfig; //!< Obiekt konfiguracyjny.
        Data::Bag mData; //!< Dane uczące

		unsigned mInputs, //!< Liczba zmiennych wejściowych modelu.
				 mOutputs, //!< Liczba zmiennych wyjściowych modelu.
				 mMaxOrder; //!< Maksymalny rząd obiektu.
	};
}
