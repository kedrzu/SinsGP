#include "stdafx.h"
#include <fstream>
#include <cstdlib> // using atof()
#include <boost/algorithm/string/replace.hpp> // replacing
#include <boost/algorithm/string.hpp> // splitting strings
#include <cmath> // pierwiastkowanie
#include "SinsGP.h"

using namespace std;
using namespace SinsGP;


Data::Data(Config& config, boost::filesystem::path file) :
	mConfig(config), mFile(file), mOutStats(config.getOutputs().size())
{
	
	ifstream ifs;
    ifs.open(file.string().c_str());
    if (ifs.fail())  { // check if file was 
		throw runtime_error("Nie udalo sie otworzyc pliku");
	}
	string line; // buffer string
    int cols = 0; // counter of file columns
	
	// odczyt pliku linijka po linijce
	while( getline(ifs, line) && ifs.good() )
    {
		// aktualny wiersz
		int rowNum = this->size();

		vector<string> strs; // buffer vector of strings
		boost::split(strs, line, boost::is_any_of("\t")); // split line into vector of strings


		// odczyt liczby kolumn, czy zgadza się z ustawieniami
		if(cols==0) {
			cols = strs.size();
            if(strs.size() != mConfig.getInputs().size() + mConfig.getOutputs().size()) {
				ifs.close();
                string err = "Zla liczba zmiennych ("+ Beagle::int2str(strs.size()) + ")";
                throw runtime_error(err.c_str());
			}

		} 
		// czy liczba kolumn w każdym wierszu jest taka sama
		else if(strs.size() != cols) {
			ifs.close();
			throw runtime_error("Plik zawiera bledy");
		}

		// nowy wiersz danych
		DataRow row(mConfig.getInputs().size(), mConfig.getOutputs().size());

		// odczyt zmiennej czasowej
        row.t = this->size()*config.getSamplingTime();

		// iteracja po wejściach
		for(unsigned i=0; i<mConfig.getInputs().size(); i++) {
            boost::replace_all(strs[i], ",", "."); // replace commas
            double val = atof(strs[i].c_str()); // string to Double
			row.u[i] = val; // place in vector in proper column
		}

		// iteracja po wyjściach
		for(unsigned i=0; i<mConfig.getOutputs().size(); i++) {
            boost::replace_all(strs[i+mConfig.getInputs().size()], ",", "."); // replace commas
            double val = atof(strs[i+mConfig.getInputs().size()].c_str()); // string to Double
			row.y[i] = val; // place in vector in proper column
			
			// obliczenia parametrów statystycznych
			if(mConfig.getContinous()) {
				if(rowNum > 0) {
                    double dt = config.getSamplingTime(); // przyrost czasu
					double yPrev = (*this)[rowNum-1].y[i]; // poprzednia wartość y
					double dy = val - yPrev; // przyrost y
					// aproksymacja prostokątami w przód
					if(mConfig.getInterpolation() == Config::rectangular) {
						mOutStats[i].integral += yPrev*dt;
						mOutStats[i].integralSquare += yPrev*yPrev*dt;
					}
					// aproksymacja trapezoidalna (domyślna)
					else {
						mOutStats[i].integral += (yPrev + val)/2*dt;
						mOutStats[i].integralSquare += dt*(1/3*dy*dy+dy*yPrev+yPrev*yPrev);
					}
				}
			} else {
				mOutStats[i].integral += val;
				mOutStats[i].integralSquare += val*val;
			}
			
		}
		this->push_back(row);
		
    }
    ifs.close(); // close the file
	if(this->size() == 0) {
		throw runtime_error("Plik nie zawiera danych");
    }
    mDuration = (*this)[this->size()-1].t;
	for(unsigned i=0; i<mConfig.getOutputs().size(); i++) {
		// wartość skuteczna
		mOutStats[i].RMS = sqrt(mOutStats[i].integralSquare/mDuration);
		// średnia
		mOutStats[i].avg = mOutStats[i].integral / mDuration;
		//odchylenie standardowe
		double devSum = 0;
		if(mConfig.getContinous()) {
			for(unsigned j=1; j<this->size(); j++) {
                double dt = config.getSamplingTime(); // przyrost czasu
				double y = (*this)[j].y[i]; // obecna wartość y
				double dev = y-mOutStats[i].avg; // obecne odchylenie od średniej
				double yPrev = (*this)[j-1].y[i]; // poprzednia wartość y
				double devPrev = yPrev-mOutStats[i].avg; // poprzednie odchylenie od średniej
				double ddev = dev-devPrev; // zmiana odchylenia
				// aproksymacja prostokątami w przód
				if(mConfig.getInterpolation() == Config::rectangular) {
					devSum += devPrev*devPrev*dt;
				}
				// aproksymacja trapezoidalna (domyślna)
				else {
					devSum += dt*(1/3*ddev*ddev+ddev*devPrev+devPrev*devPrev);
				}
			} 
		} else {
			for(unsigned j=0; j<this->size(); j++) {
				double y = (*this)[j].y[i]; // obecna wartość y
				double dev = y-mOutStats[i].avg; // obecne odchylenie od średniej
				devSum += dev*dev;
			}
		}
		mOutStats[i].std = sqrt(devSum/mDuration);
	}
}

Data::~Data(void)
{
}

