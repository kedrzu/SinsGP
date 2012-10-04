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
    mConfig(config), mFile(file), mInputs(config.getInputs().size()),
    mOutputs(config.getOutputs().size()), mRows(0), mDuration(0)
{
	
	ifstream ifs;
    ifs.open(file.string().c_str());
    if (ifs.fail())  { // check if file was 
		throw runtime_error("Nie udalo sie otworzyc pliku");
	}
	string line; // buffer string
    unsigned cols = 0; // counter of file columns
	
	// odczyt pliku linijka po linijce
	while( getline(ifs, line) && ifs.good() )
    {
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

		// iteracja po wejściach
		for(unsigned i=0; i<mConfig.getInputs().size(); i++) {
            boost::replace_all(strs[i], ",", "."); // replace commas
            double val = atof(strs[i].c_str()); // string to Double
            mInputs[i].push_back(val);
		}

		// iteracja po wyjściach
		for(unsigned i=0; i<mConfig.getOutputs().size(); i++) {
            boost::replace_all(strs[i+mConfig.getInputs().size()], ",", "."); // replace commas
            double val = atof(strs[i+mConfig.getInputs().size()].c_str()); // string to Double
            mOutputs[i].push_back(val);
        }

        ++mRows;
		
    }
    ifs.close(); // close the file
    if(mRows == 0) {
		throw runtime_error("Plik nie zawiera danych");
    }
    for(unsigned i=0; i<mOutputs.size(); ++i) {
        mInputs[i].evalStats();
    }
    for(unsigned i=0; i<mOutputs.size(); ++i) {
        mOutputs[i].evalStats();
    }
    mDuration = mRows*mConfig.getSamplingTime();
}

Data::~Data(void)
{
}

