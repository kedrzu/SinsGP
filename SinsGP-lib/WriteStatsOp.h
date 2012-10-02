#pragma once

#include <map>
#include "boost/array.hpp"
#include "Status.h"

namespace SinsGP {
	//! Klasa reprezentuje operator Open Beagle służący do bieżącej oceny procesu uczenia
	/*!
		Klasa dziedziczy po Beagle::Operator.
		W modelu przyjętym w Open Beagle istnieją tzw operatory, czyli klasy, które
		przeprowadzają pewne działania na populacji w każdej generacji. Możliwe jest
		zdefiniowanie własnych operatorów i w ten sposób dowolne wpływanie na proces
		uczenia.
		Klasa rejestruje parametry statystyczne populacji (funkcję przystosowania, 
		rozmiary drzew osobników) w każdym pokoleniu w celu późniejszego wypisania
		do pliku CSV i/lub 	wyrysowania na wykresie. Jest to konieczne, ponieważ 
		ocena postępów algorytmu uczącego na przestrzeni kolejnych generacji jest
		podstawą do odpowiedniego doboru parametrów algorytmu genetycznego.
		Podstawową metodą klasy jest WriteStatsOp::operate(). Jest to medota wywoływana
		przez Evolver dla każdej populacji (może być ich wiele) w każdej generacji.
		Dane statystyczne są wyliczane przez Open Beagle, jednak nie istnieje domyślny
		mechanizm zapisywania ich w postaci CSV, najłatwiejszej do odczytania przez
		człowieka. Klasa jedynie agreguje te dane i wypisuje je w odpowiedniej postaci.
		Agregacji danych dokonuje się poprzez zastosowanie mapy, której kluczami są 
		wskaźniki na obiekty populacji (tzw. deme), zaś wartościami obiekty klasy
		WriteStatsOp::Stats, reprezentującej statystyki populacji w procesie uczenia.
	*/
	class WriteStatsOp : public Beagle::Operator
	{
	public:
		//! Alokator danych (wymagany przez Open Beagle)
		typedef Beagle::AllocatorT<WriteStatsOp,Beagle::Operator::Alloc> Alloc;
		//! Inteligentny uchwyt (wymagany przez Open Beagle)
		typedef Beagle::PointerT<WriteStatsOp,Beagle::Operator::Handle> Handle;
		//! Domyślny kontener (wymagany przez Open Beagle)
		typedef Beagle::ContainerT<WriteStatsOp,Beagle::Operator::Bag> Bag;

		/*!
			\brief Wiersz statystyk
			
			Może zawierać dowolną liczbę zmiennych, z których każda
			opisana jest jej wartością średnią, odchyleniem standardowym,
			wartością maksymalną, minimalną.
		*/
        typedef std::vector<boost::array<double, 4> > Row;

		//! Klasa reprezentuje wyniki statystyczne dla jednej populacji
		/*!
			Dla każdej z populacji dane statystyczne zapisane są w postaci listy wierszy.
			Liczba i rodzaj rejestrowanych zmiennych zależą od ustawień algorytmu
			genetycznego, konkretnie od rodzaju zastosowanej miary przystosowania 
			(prostej lub tzw. przystosowania Kozy).
		*/
		class Stats {
			friend class WriteStatsOp;
		public:
			//! Konstruktor klasy
			Stats(Config* config);
		private:
			Config* mConfig; //!< Obiekt konfiguracyjny
			std::list<Row> mData; //!< Dane statystyczne
		};

		/*!
			\brief Konstruktor klasy
			\param config Obiekt konfiguracyjny
			\param inName Nazwa operatora (wymagana przez Open Beagle)
		*/
		explicit WriteStatsOp(Config* config, Beagle::string inName="WriteStatsOp");
		virtual ~WriteStatsOp() {}

		/*!
			\brief Podstawowa metoda operatora w Open Beagle
			\param ioDeme Obiekt populacji Open Beagle
			\param ioContext Obiekt Open Beagle reprezentujący aktualny stan procesu uczenia.
		*/
		virtual void operate(Beagle::Deme& ioDeme, Beagle::Context& ioContext);
		
		/*!
			\brief Zapisywanie danych do pliku
			\param vivarium Zbiór populacji, których dane statystyczne mają być zapisane
            \param status Status procesu optymalizacji
		*/
        virtual void exec(Beagle::Vivarium::Handle vivarium, Status& status);

        /*!
         *  \brief Czyszczenie statystyk
         */
        void clear();
	 
	private:
		Config* mConfig; //!< Obiekt konfiguracyjny
		std::map<Beagle::Deme*, Stats*> mStats; //!< Dane statystyczne populacji
		std::vector<std::string> mHeaders; //!< Nazwy wskaźników statystycznych
	};
}
