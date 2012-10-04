#pragma once


namespace SinsGP{
//! Klasa konfiguracyjna.
/*!
 *  Odpowiada za parsowanie konfiguracyjnego pliku XML.
 *  Zawiera informacje takie jak: parametry szukanego modelu,
 *  nazwy zmiennych występujących w modelu, ścieżki do wszystkich plików projektu.
 *  Jest singletonem i jest przekazywana jako argument konstruktora w każdym tworzonym obiekcie.
 */
class Config
{

public:

    //! Rodzaje plików wynikowych
    /*!
            Określa różne rodzaje plików wynikowych. Stosowany jako flagi bitowe.
    */
    enum Result {
        XML = 1,
        SVG = 2,
        PNG = 4,
        CSV = 8,
        PLOT = 16,
        EQUATIONS = 32
    };

    //! Rodzaje interpolacji
    /*!
            W przypadku szukanego modelu ciągłego dyskretne sygnały uczące należy
            interpolować funkcją ciągłą.
    */
    enum Interpolation {
        rectangular, //!< Interpolacja prostokątami w przód
        trapezoidal //!< Interpolacja trapezami
    };

    //! Ścieżki do plików.
    /*!
            Zawiera wszystkie ścieżki do plików używanych w programie.
    */
    struct Paths {
        boost::filesystem::path
        program, //!< Ścieżka programu
        problem, //!< Folder z problemem
        config, //!< Plik konfiguracyjny
        data, //!< Folder z danymi uczącymi
        test, //!< Folder z danymi testowymi
        beagleConfig, //!< Plik konfiguracyjny Open Beagle
        result, //!< Folder z wynikami
        run, //!< Folder z wynikami uruchomienia
        temp, //!< Folder tymczasowy
        tree2svg, //!< Plik konwertera XML->SVG
        rasterizer; //!< Plik konwertera SVG->PNG

        //! Odczyt listy plików z danymi uczącymi
        /*!
         *  Przeszukuje folder z danymi uczącymi i zwraca listę plików.
         *  \return Lista plików z danymi uczącymi.
         */
        std::vector<boost::filesystem::path> getDataFiles() const;
        //! Odczyt listy plików z danymi testowymi
        /*!
         *  Przeszukuje folder z danymi uczącymi i zwraca listę plików.
         *  \return Lista plików z danymi testowymi.
         */
        std::vector<boost::filesystem::path> getTestFiles() const;
    };

    /*!
            Parsuje plik konfiguracyjny podany jako argument przy uruchamianiu programu.

            \param argc Liczba parametrów podanych przy uruchomieniu programu (argument funkcji main())
            \param argv Wartości parametrów podanych przy uruchomieniu programu (argument funkcji main())
            \throw std::runtime_error W razie wystąpienia błędu parsowania pliku konfiguracyjnego
         */
    Config(int argc, char *argv[]);
    virtual ~Config(void);

    /*!
     *  \brief  Zwraca nazwy zmiennych wejściowych
     *  \return Config::mInputs Lista zmiennych wejściowych
     */
    const Beagle::GP::Primitive::Bag& getInputs() const { return mInputs; }
    /*!
     *  \brief Zwraca nazwę zmiennej wejściowej
     *  \param i Numer wejścia (liczony od 0)
     *  \return Config::mInputs[i] Zmienna wejściowa
     */
    Beagle::GP::Primitive::Handle getInputs(int i) const { return mInputs[i]; }
    /*!
     *  \brief Zwraca nazwy zmiennych wyjściowych
     *  \return Config::mOutputs Lista zmiennych wyjściowych
     */
    const std::vector<Beagle::string>& getOutputs() const { return mOutputs; }
    /*!
            \brief Zwraca nazwę zmiennej wyjściowej
            \param i Numer wyjścia (liczony od 0)
            \return Config::mOutputs[i] Nazwa zmiennej wyjściowej
         */
    const Beagle::string& getOutputs(int i) const { return mOutputs[i]; }
    /*!
            \brief Zwraca nazwy zmiennych stanu
            \return Config::mStateVar Lista zmiennych stanu
         */
    const Beagle::GP::Primitive::Bag& getStateVar() const { return mStateVar; }
    /*!
            \brief Zwraca nazwę zmiennej stanu
            \param i Numer zmiennej stanu (liczony od 0)
            \return Config::mStateVar[i] Zmienna stanu
         */
    Beagle::GP::Primitive::Handle getStateVar(int i) const { return mStateVar[i]; }
    /*!
     *  \brief  Zwraca tablicę określającą, czy zmienne stanu są pochodnymi innych
     *  \return Config::mDerivative
     */
    const std::vector<bool>& isDerivative() const { return mIsDerivative; }
    /*!
     *  \brief Zwraca, czy dana zmienna stanu jest pochodną poprzedniej
     *  \param i Numer zmiennej stanu (liczony od 0)
     *  \return Config::mDerivative[i]
     */
    bool isDerivative(int i) const { return mIsDerivative[i]; }
    /*!
     *  \brief  Zwraca tablicę określającą, czy zmienne stanu są obserwowalne
     *  \return Config::mIsObservable
     */
    const std::vector<bool>& isObservable() const { return mIsObservable; }
    /*!
     *  \brief Zwraca, czy dana zmienna stanu jest obserwowalna
     *  \param i Numer zmiennej stanu (liczony od 0)
     *  \return Config::mIsObservable[i]
     */
    bool isObservable(int i) const { return mIsObservable[i]; }
    /*!
     *  \brief Zwraca tablicę numerów obserwowalnych zmiennych stanu
     *  \return Config::mObservables
     */
    const std::vector<unsigned>& getObservables() { return mObservables; }
    /*!
     *  \brief Numer i-tej obserwowalnej zmiennej stanu.
     *  \return Config::mObservables[i]
     */
    unsigned getObservables(int i) { return mObservables[i]; }
    /*!
     *  \brief Zwraca tablicę równań stanu będących pochodnymi zmiennych
     *  \return Config::mDerivatives
     */
    const std::vector<unsigned>& getDerivatives() { return mDerivatives; }
    /*!
     *  \brief Numer i-tego równania stanu będącego pochodną zmiennej stanu.
     *  \return Config::mDerivatives[i]
     */
    unsigned getDerivatives(int i) { return mDerivatives[i]; }
    /*!
     *  \brief Zwraca minimalny rząd modelu
     *  \return Config::mMinOrder Minimalny rząd modelu
     */
    unsigned getMinOrder() const { return mMinOrder; }
    /*!
            \brief Zwraca maksymalny rząd modelu
            \return Config::mMaxOrder Maksymalny rząd modelu
    */
    unsigned getMaxOrder() const { return mMaxOrder; }
    /*!
            \brief Zwraca konfigurację plików wynikowych modelu
            \return Config::mResIndModel Flagi bitowe zawierające Config::Result
         */
    int getResultIndModel() const { return mResIndModel; }
    /*!
            \brief Zwraca konfigurację plików wyników uczenia
            \return Config::mResIndData Flagi bitowe zawierające Config::Result
         */
    int getResultIndData() const { return mResIndData; }
    /*!
            \brief Zwraca konfigurację plików wyników testowania
            \return Config::mResIndTest Flagi bitowe zawierające Config::Result
         */
    int getResultIndTest() const { return mResIndTest; }
    /*!
            \brief Zwraca konfigurację plików statystyk uczenia
            \return Config::mResStats Flagi bitowe zawierające Config::Result
    */
    int getResultStats() const { return mResStats; }
    /*!
            \brief Zwraca liczbę uruchomień optymalizacji
            \return Config::mRuns
    */
    unsigned getRuns() const { return mRuns; }
    /*!
            \brief Określa, czy szukany model jest ciągły
            \return Config::mContinous
    */
    bool getContinous() const { return mContinous; }
    /*!
            \brief Zwraca rodzaj zastosowanej interpolacji
            \return Config::mInterpolation
    */
    Interpolation getInterpolation() const { return mInterpolation; }
    /*!
            \brief Zwraca ścieżki dostępu do wszystkich plików używanych w programie
            \return Config::mPaths
    */
    const Paths& getPaths() const { return mPaths; }
    /*!
     *      \brief Zwraca czas próbkowania
     *      \return double mSamplingTime Czas próbkowania
     */
    double getSamplingTime() const { return mSamplingTime; }

private:
    std::vector<Beagle::string> mOutputs; //!< Nazwy zmiennych wyjściowych
    Beagle::GP::Primitive::Bag mInputs; //!< Zmienne wejściowe
    Beagle::GP::Primitive::Bag mStateVar; //!< Zmienne stanu
    std::vector<bool>  mIsObservable; //!< Tablica określająca, czy dana zmienna stanu jest obserwowalna
    std::vector<bool>  mIsDerivative; //!< Tablica określająca, czy dana zmienna stanu jest pochodną innej
    std::vector<unsigned> mObservables; //!< Tablica określająca kolejno numery obserwowalnych zmiennych stanu
    std::vector<unsigned> mDerivatives; //!< Tablica określająca kolejno numery zmiennych stanu będących pochodnymi
    unsigned mMinOrder; //!< Minimalny szukany rząd
    unsigned mMaxOrder; //!< Maksymalny szukany rząd
    bool mContinous; //!< Czy model jest z czasem ciągłym
    bool mError; //!< Określa czy wystąpił błąd podczas parsowania
    Interpolation mInterpolation; //!< Rodzaj zastosowanej interpolacji sygnału (tylko układy ciągle)
    std::string mException; //!< Lista błędów do wyrzucenia jako wyjątek, jeżeli był błąd
    Paths mPaths; //!< Ścieżki dostępu
    int mResIndModel; //!< Wynikowa reprezentacja modelu w postaci flag bitów
    int mResIndData; //!< Wynikowa reprezentacja jakości uczenia w postaci flag bitów
    int mResIndTest; //!< Wynikowa reprezentacja jakości uczenia w postaci flag bitów
    int mResStats; //!< Wynikowa reprezentacja statystyk uczenia
    unsigned mRuns; //!< Liczba uruchomień optymalizacji genetycznej
    double mSamplingTime; //!< Czas próbkowania danych

    // FUNCTIONS
    inline void error(std::string e);
};
}
