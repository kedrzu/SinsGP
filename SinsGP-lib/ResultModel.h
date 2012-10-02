#pragma once
#include "Config.h"
#include "Status.h"

namespace SinsGP {
	//! Klasa odpowiada za pojedynczy wynik uczenia.
	/*!
		Klasa dziedziczy z Result::Bag
		Otrzymane w wyniku algorytmu genetycznego osobniki mają postać pewnej ilości
		drzew. Należy przekształcić je do postaci czytelnej dla człowieka, tj
		z tych drzew wybrać równania stanu i wyjść i zapisać w postaci naturalnej (równań).
		Ponadto należy wygenerować wyniki statystyczne uczenia. Reprezentowane są one
		z jednej strony przez obiekty typu Result, z których każdy odpowiada dokładnym
		wynikom predykcji dla jednego przypadku uczącego bądź testowego, z drugiej zaś
		przez zestawienie danych statystycznych z wszystkich przypadków uczących i testowych
		w jednej tabeli w postaci miary błędu predykcji, czy współczynników korelacji.
	*/
	class ResultModel : public Result::Bag
	{
	public:
		/*!
			\brief Konstruktor klasy
			\param config Obiekt konfiguracyjny
            \param status Status procesu optymalizacji
		*/
        ResultModel(Config* config, Beagle::GP::Individual::Handle indi, Status& status);
		
		/*!
			\brief Przekształcanie drzewa osobnika do postaci wyrażenia algebraicznego
			\param tree Obiekt drzewa

			Ze względu na dość trudny do odczytania z kodu Open Beagle sposób zapisu
			hierarchicznej struktury drzewa, przekształcane jest ono do postaci
			XML (funkcjonalność wbudowana w każdy obiekt Open Beagle), po czym 
			jest parsowane. Następnie do najwyższego węzła drzewa zastosowana jest
			pomocnicza rekurencyjna funkcja flattenXML().
		*/
		static std::string flattenTree(Beagle::GP::Tree::Handle tree);
	
		/*!
			\brief Rekurencyjna funkcja parsująca drzewo XML
			\param node Węzeł, który ma zostać zinterpretowany
			\return String zawierający wyrażenie algebraiczne

			Funkcja odczytuje węzeł drzewa (tag XML) i w zależności od jego typu
			generuje string z wyrażeniem algebraicznym. Np dla operacji dodawania
			(tag ADD) generowany jest string "(" + flattenXML(child1) + "+" + flattenXML(child2) + ")".
			W ten sposób uruchomienie funkcji dla najwyższego w hierarchi węzła powoduje
			rekurencyjne obejście całego drzewa.
		*/
		static std::string flattenXML(PACC::XML::Node* node);

		/*! 
			\brief Zapis danych do odpowiednich plików

			Funkcja parsuje wszystkie drzewa osobnika przy pomocy funkcji
			parseTree(), generuje równania stanu i wyjść i zapisuje do pliku tekstowego
			w folderze z wynikami osobnika.

			Oblicza parametry statystyczne uczenia dla wszystkich przypadków uczących i
			testowych oraz zapisuje w odpowiednim wynikowym pliku CSV.

			Uruchamia medotę Result::exec() dla każdego wyniku w celu generacji
			plików wynikowych (csv, wykresów) dla każdego z przypadków uczących i testowych.
		*/
        void exec();

        /*!
            \brief Zwraca status
        */
        Status& getStatus() const { return *mStatus; }

		/*!
			\brief Zwraca osobnika
			\return mIndi Osobnik
		*/
        Beagle::GP::Individual::Handle getIndividual() const { return mIndi; }

        ~ResultModel(void) {}
	private:
		Config* mConfig; //!< Obiekt konfiguracyjny
        Beagle::GP::Individual::Handle mIndi; //!< Testowany osobnik
        Status* mStatus; //!< Status procesu optymalizacji
	};
}
