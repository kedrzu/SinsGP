#pragma once

namespace SinsGP {
	//! Klasa reprezentuje jeden wiersz danych uczących
	/*!
	 *	Wiersz danych uczących zawiera wartość zmiennej czasowej oraz wartości sygnałów
	 *	wejściowych i wyjściowych.
	 *	Zmienne przechowujące dane dostępne są publicznie. Klasa odpowiada także
	 *	za przetworzenie wartości zmiennych do postaci wiersza pliku CSV oraz wypisanie
	 *	nagłówków dla pliku CSV.
	 */
	class DataRow
	{
	public:

		/*!
		 *	\brief Konstruktor domyślny.
		 */
		DataRow();
		/*!
		 *	\brief Konstruktor inicjujący wektory sygnałów odpowiedniej wielkości.
		 *	\param inputs Liczba sygnałów wejściowych
		 *	\param outputs Liczba sygnałów wyjściowych
		 */
		DataRow(int inputs, int outputs);

		virtual ~DataRow(void);

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

		double t; //!< Zmienna czasowa
		std::vector<double> u; //!< Sygnały wejściowe
		std::vector<double> y; //!< Sygnały wyjściowe

		friend std::ostream& operator<<(std::ostream& out, const DataRow& r);
	};
}
