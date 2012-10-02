#pragma once

#include "DataRow.h"

namespace SinsGP {
	//! Klasa reprezentuje jeden wiersz danych danych wynikowych
	/*!
		Wiersz danych wynikowych zawiera wartość zmiennej czasowej, wartości sygnałów
		wejściowych i wyjściowych, zmiennych stanu oraz błędy predykcji sygnałów
		wyjściowych: prosty i znormalizowany.
		Zmienne przechowujące dane dostępne są publicznie. Klasa odpowiada także
		za przetworzenie wartości zmiennych do postaci wiersza pliku CSV oraz wypisanie
		nagłówków dla pliku CSV.
	 */
	class ResultRow : public DataRow
	{
	public:

		std::vector<double> x; //!< Wektor stanu
		std::vector<double> yModel; //!< Wektor wyjścia modelu
		std::vector<double> e; //!< Wektor błędu

		/*!
			Konstruktor domyślny.
		*/
		ResultRow();
		/*!
			\brief Konstruktor inicjujący wektory odpowiedniej wielkości.
			\param inputs Liczba sygnałów wejściowych
			\param outputs Liczba sygnałów wyjściowych
			\param order Rząd modelu
		 */
		ResultRow(int inputs, int outputs, int order);
		~ResultRow(void);

		/*!
		 *	\brief Generuje nagłówki do zapisu do pliku CSV
		 *	\return String zawierający nazwy zmiennych rozdzielone tabulatorami.
		 */
		virtual std::string headers();
		
		/*!
		 *	\brief Generuje string z wartościami zmiennych do zapisu do pliku CSV.
		 *	\return String zawierający wartości zmiennych rozdzielone tabulatorami.
		 */
		virtual std::string string();


		friend std::ostream& operator<<(std::ostream& out, const ResultRow& r);

	};
}

