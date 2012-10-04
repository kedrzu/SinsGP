#include "stdafx.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp> // replacing
#include <iostream>
#include "SinsGP.h"

using namespace SinsGP;
using namespace std;
using namespace boost::filesystem;
using namespace Beagle;

Config::Config(int /* argc */, char *argv[]) :
    mException(""),
    mError(false),
    mContinous(false)
{
    mPaths.config = path(argv[1]);
    mPaths.program = path(argv[0]).parent_path();
    mPaths.tree2svg = mPaths.program / path("bin") / path("tree2svg.exe");
    mPaths.rasterizer = mPaths.program / path ("bin") / path("batik-rasterizer.jar");

    // sprawdzanie, czy istnieje plik programu tree2svg
    if(!exists(mPaths.tree2svg) || !is_regular_file(mPaths.tree2svg)) {
        error("Nie znaleziono pliku programu tree2svg \"" + mPaths.tree2svg.string() + "\"");
        throw runtime_error(mException.c_str()); // je�eli nie ma pliku to nie ma dalej co sprawdza�
    }

    // sprawdzanie, czy istnieje plik konfiguracyjny
    if(!exists(mPaths.config) || !is_regular_file(mPaths.config)) {
        error("Nie znaleziono pliku konfiguracyjnego \"" + mPaths.config.string() + "\"");
        throw runtime_error(mException.c_str()); // je�eli nie ma pliku to nie ma dalej co sprawdza�
    }
    mPaths.problem = mPaths.config.parent_path();

    // parsowanie pliku konfiguracyjnego
    PACC::XML::Document config; // obiekt dokumentu XML
    config.parse(mPaths.config.string()); // wczytujemy plik konfiguracji
    PACC::XML::Finder finder(config.getFirstDataTag()); // obiekt do wyszukiwania tag�w
    PACC::XML::Iterator it; // iterator do kt�rego �adowane s� wyniki

    // czy model jest z czasem ci�g�ym?
    it = finder.find("//model/type/*");
    if(it) { // czy zwr�cono prawid�owy pointer?
        if(it->getValue() == "continous")
            mContinous = true;
        else if(it->getValue() == "discrete")
            mContinous = false;
        else
            error("Podano nieprawid�owy typ modelu (discrete/continous) [SinsGP/model/type]");
    } else {
        error("Nie podano typu modelu (discrete/continous) [SinsGP/model/type]");
    }

    // odczyt czasu pr�bkowania
    it = finder.find("//model/sampling-time/*");
    if(it) { // czy zwr�cono prawid�owy pointer?
        mSamplingTime = str2dbl(it->getValue());
    } else {
        error("Nie podano czasu pr�bkowania danych [SinsGP/model/sampling-time]");
    }

    // odczyt liczby zmiennych wej�ciowych
    it = finder.find("//model/inputs/*");
    if(it) { // czy zwr�cono prawid�owy pointer?
        unsigned long vars = str2uint(it->getValue());
        mInputs.clear();
        for(unsigned long i=0; i<vars; i++)
            mInputs.push_back(new GP::TokenT<Double>("u"+int2str(i)));
    } else {
        error("Nie podano liczby zmiennych wejsciowych [SinsGP/model/inputs]");
    }

    // odczyt liczby zmiennych wyj�ciowych
    it = finder.find("//model/outputs/*");
    if(it) { // czy zwr�cono prawid�owy pointer?
        unsigned long vars = str2uint(it->getValue());
        mOutputs.clear();
        for(unsigned long i=0; i<vars; i++)
            mOutputs.push_back("y"+int2str(i));
    } else {
        error("Nie podano liczby zmiennych wyjsciowych [SinsGP/model/outputs]");
    }

    // odczyt maksymalnej liczby zmiennych stanu
    it = finder.find("//model/max-order/*");
    if(it) { // czy zwr�cono prawid�owy pointer?
        unsigned long vars = str2uint(it->getValue());
        mStateVar.clear();
        for(unsigned long i=0; i<vars; i++)
            mStateVar.push_back(new GP::TokenT<Double>("x"+int2str(i)));
        mMaxOrder = (unsigned)vars;
        mIsDerivative.resize(mMaxOrder, false);
        mIsObservable.resize(mMaxOrder, false);
    } else {
        error("Nie podano maksymalnego rzedu obiektu [SinsGP/model/max-order]");
    }

    // odczyt minimalnej liczby zmiennych stanu
    it = finder.find("//model/min-order/*");
    if(it) { // czy zwr�cono prawid�owy pointer?
        unsigned long vars = str2uint(it->getValue());
        mMinOrder = (unsigned)vars;
    } else {
        error("Nie podano minimalnego rzedu obiektu [SinsGP/model/min-order]");
    }

    // zmienne stanu b�d�ce pochodnymi innych
    for(it = finder.find("//model/state-vars/derivative/*"); it; it = finder.findNext()) {
        if(!it->getValue().empty()) { // czy znacznik nie jest pusty?
            unsigned der = str2uint(it->getValue());
            if (der >= mMinOrder)
                error("Nie mo�na ustali� r�wnania stanu nr " + int2str(der) + " jako pochodnej. Zbyt wysoki numer zmiennej stanu.");
            else if(der == 0)
                error("Nie mo�na ustali� r�wnania stanu nr 0 jako pochodnej (r�wnanie stanu jest zawsze pochodn� poprzedniej zmiennej stanu).");
            else if (mIsDerivative[der])
                error("Nie mo�na ustali� r�wnania stanu nr " + int2str(der) + " jako pochodnej. R�wnanie zosta�o ju� wcze�niej ustalone.");
            else {
                mIsDerivative[der] = true;
                mDerivatives.push_back(der);
            }
        }
    }


    // obserwowalne zmienne stanu
    for(it = finder.find("//model/state-vars/observable/*"); it; it = finder.findNext()) {
        if(!it->getValue().empty()) { // czy znacznik nie jest pusty?
            unsigned obs = str2uint(it->getValue());
            bool e = false;
            if (obs >= mMinOrder) {
                e = true;
                error("Nie mo�na ustali� zmiennej stanu nr " + int2str(obs) + " jako obserwowalnej. Zbyt wysoki numer zmiennej stanu w stosunku do minimalnego rzedu modelu.");
            }
            if (mObservables.size() == mOutputs.size()) {
                e = true;
                error("Nie mo�na ustali� zmiennej stanu nr " + int2str(obs) + " jako obserwowalnej. Zbyt wysoki numer zmiennej stanu w stosunku do liczby zmiennych wyj�ciowych.");
            }
            if (mIsObservable[obs]) {
                e = true;
                error("Nie mo�na ustali� zmiennej stanu nr " + int2str(obs) + " jako obserwowalnej. R�wnanie zosta�o ju� wcze�niej ustalone.");
            }
            if (!e) {
                mIsObservable[obs] = true;
                mObservables.push_back(obs);
            }
        }
    }

    // rodzaj interpolacji
    if(mContinous) {
        it = finder.find("//model/interpolation/*");
        if(it) { // czy zwr�cono prawid�owy pointer?
            if(it->getValue() == "rectangular")
                mInterpolation = Config::rectangular;
            else if(it->getValue() == "trapezoidal")
                mInterpolation = Config::trapezoidal;
            else {
                error("SinsGP::Config: Podano nieprawidlowy rodzaj interpolacji (trapezoidal/rectangular) [SinsGP/model/interpolation].");
            }
        } else {
            // domy�lnie trapezoidalna
            error("SinsGP::Config: Nie podano rodzaju interpolacji (trapezoidal/rectangular) [SinsGP/model/interpolation].");
        }
    }

    // folder z danymi ucz�cymi
    it = finder.find("//files/data-dir/*");
    if (it) { // czy zwr�cono prawid�owy pointer?
        if(!it->getValue().empty()) { // czy znacznik nie jest pusty?
            mPaths.data = mPaths.problem / path(it->getValue());

            // sprawdzanie, czy istnieje folder z danymi ucz�cymi
            if(!(exists(mPaths.data) && is_directory(mPaths.data))) {
                error("Nie znaleziono folderu z danymi uczacymi \"" + mPaths.data.string() + "\"");
            }
        } else {
            error("Nie podano folderu z danymi ucz�cymi");
        }
    } else {
        error("Nie podano folderu z danymi ucz�cymi");
    }

    // folder z danymi testowymi
    it = finder.find("//files/test-dir/*");
    if(it) { // czy zwr�cono prawid�owy pointer?
        if(!it->getValue().empty()) { // czy znacznik nie jest pusty?
            mPaths.test = mPaths.problem / path(it->getValue());
        }
    }

    // folder z wynikami oblicze�
    it = finder.find("//files/result-dir/*");
    if(it) { // czy zwr�cono prawid�owy pointer?
        if(!it->getValue().empty()) { // czy znacznik nie jest pusty?
            mPaths.result = mPaths.problem / path(it->getValue()); // folder z wynikami
            create_directory(mPaths.result); // utw�rz je�eli nie istnieje
            PACC::Date date; // obecna data
            std::string dateStr = date.get("%Y-%m-%d %H-%M-%S");
            mPaths.run = mPaths.result / path(dateStr);
            create_directory(mPaths.run);
            mPaths.temp = mPaths.program / path("temp") / path(dateStr);
            create_directory(mPaths.program / path("temp"));
            create_directory(mPaths.temp);
        } else {
            error("Nie podano folderu do umieszczenia wynik�w oblicze�");
        }
    } else {
        error("Nie podano folderu do umieszczenia wynik�w oblicze�");
    }

    // plik konfiguracji Beagle
    it = finder.find("//files/beagle-config/*");
    if(it) { // czy zwr�cono prawid�owy pointer?
        if(!it->getValue().empty()) { // czy znacznik nie jest pusty?
            mPaths.beagleConfig = mPaths.problem / path(it->getValue());
            if(!exists(mPaths.beagleConfig) || !is_regular_file(mPaths.beagleConfig)) {
                error("Nie znaleziono pliku konfiguracji Open Beagle \"" + mPaths.beagleConfig.string() + "\"");
            }
        } else {
            mPaths.beagleConfig = path();
        }
    } else {
        mPaths.beagleConfig = path();
    }

    // odczyt liczby uruchomie� optymalizacji
    it = finder.find("//runs/*");
    if(it) { // czy zwr�cono prawid�owy pointer?
        unsigned long vars = str2uint(it->getValue());
        mRuns = (unsigned)vars;
    } else {
        error("Nie podano liczby uruchomie� optymalizacji");
    }

    // konfiguracja plik�w wynikowych modelu osobnik�w
    for(it = finder.find("//result/model/*"); it; it = finder.findNext()) {
        std::string str = it->getValue(); // odczyt
        boost::algorithm::trim(str); // obcinanie bia�ych znak�w
        boost::algorithm::to_lower(str); // na ma�e litery
        if(str == "tree-xml") {
            mResIndModel |= XML;
        } else if(str == "tree-svg") {
            mResIndModel |= SVG;
        } else if(str == "tree-png") {
            mResIndModel |= PNG;
        } else if(str == "equations") {
            mResIndModel |= EQUATIONS;
        }
    }

    // konfiguracja plik�w wynikowych jako�ci uczenia osobnik�w (dla danych ucz�cych)
    for(it = finder.find("//result/data/*"); it; it = finder.findNext()) {
        std::string str = it->getValue(); // odczyt
        boost::algorithm::trim(str); // obcinanie bia�ych znak�w
        boost::algorithm::to_lower(str); // na ma�e litery
        if(str == "csv") {
            mResIndData |= CSV;
        } else if(str == "plot") {
            mResIndData |= PLOT;
        }
    }

    // konfiguracja plik�w wynikowych jako�ci uczenia osobnik�w (dla danych testowych)
    for(it = finder.find("//result/test/*"); it; it = finder.findNext()) {
        std::string str = it->getValue(); // odczyt
        boost::algorithm::trim(str); // obcinanie bia�ych znak�w
        boost::algorithm::to_lower(str); // na ma�e litery
        if(str == "csv") {
            mResIndTest |= CSV;
        } else if(str == "plot") {
            mResIndTest |= PLOT;
        }
    }

    // konfiguracja plik�w wynikowych statystyk
    for(it = finder.find("//result/stats/*"); it; it = finder.findNext()) {
        std::string str = it->getValue(); // odczyt
        boost::algorithm::trim(str); // obcinanie bia�ych znak�w
        boost::algorithm::to_lower(str); // na ma�e litery
        if(str == "csv") {
            mResStats |= CSV;
        } else if(str == "plot") {
            mResStats |= PLOT;
        }
    }

    if(mError)
        throw runtime_error(mException.c_str());
}

std::vector<path> Config::Paths::getDataFiles() const {
    std::vector<path> vec;
    copy(directory_iterator(data), directory_iterator(), back_inserter(vec));
    return vec;
}

std::vector<path> Config::Paths::getTestFiles() const {
    std::vector<path> vec;

    // sprawdzanie, czy istnieje folder z danymi testowymi
    if(!exists(data) || !is_directory(data)) {
        return vec;
    }
    if(test.empty())
        return vec;
    copy(directory_iterator(test), directory_iterator(), back_inserter(vec));
    return vec;
}

void Config::error(std::string e) {
    mException += "SinsGP::Config:" + e + "\n";
    mError = true;
}


Config::~Config(void)
{
}

