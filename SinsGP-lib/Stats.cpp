#include "stdafx.h"
#include "SinsGP.h"

using namespace SinsGP;
using namespace boost::filesystem;

Stats::Stats(Config* config) : mConfig(config)
{
		mCsvPath = mConfig->getPaths().run / path("result-stats.csv");
        mCsv = new std::ofstream(mCsvPath.string().c_str());

		if(!mCsv->good())
			throw std::runtime_error("SinsGP::Stats::Stats() Nie udalo sie otworzyc do zapisu pliku " + mCsvPath.string());

		// NAGŁÓWKI
        *mCsv << "Run" << "\t" << "Model"; // numer runu osobnika
		
		// nagłowki - dane uczące
        *mCsv
            << "\t" << "overall NMSE (training)"; // średniokwadratowy błąd znormalizowany

		// nagłówki - zmienne wyjściowe
		for(unsigned i=0; i<mConfig->getOutputs().size(); i++) {
			*mCsv 
                << "\t" << mConfig->getOutputs()[i] << " NMSE (training)"
                << "\t" << mConfig->getOutputs()[i] << " MSE (training)";
		}	

		// nagłówki - dane testowe
        *mCsv
            << "\t" << "overall NMSE (testing)"; // średniokwadratowy błąd znormalizowany

		// nagłówki - zmienne wyjściowe
		for(unsigned i=0; i<mConfig->getOutputs().size(); i++) {
			*mCsv 
                << "\t" << mConfig->getOutputs()[i] << " NMSE (testing)"
                << "\t" << mConfig->getOutputs()[i] << " MSE (testing)";
		}
}

Stats::~Stats(void)
{
	mCsv->close();
	delete mCsv;
}

void Stats::addRow(const Row& row) {
	// sprawdzanie poprawności danych
    if(row.training.size() != mConfig->getOutputs().size())
        throw std::runtime_error("SinsGP::Stats::addRow() Zla liczba zmiennych w wynikach uczenia. Powinno byc "+ Beagle::int2str(mConfig->getOutputs().size()) + ", jest " + Beagle::int2str(row.training.size()));
    if(row.testing.size() != mConfig->getOutputs().size())
        throw std::runtime_error("SinsGP::Stats::addRow() Zla liczba zmiennych w wynikach testu. Powinno byc "+ Beagle::int2str(mConfig->getOutputs().size()) + ", jest " + Beagle::int2str(row.testing.size()));
		
	// nowa linijka
	*mCsv << std::endl;
	
	// DANE
    *mCsv << row.run << "\t" << row.model; // numer runu i osobnika
	
	// dane uczące
	*mCsv 
        << "\t" << row.training.getNMSE();

	// zmienne wyjściowe
	for(unsigned i=0; i<mConfig->getOutputs().size(); i++) {
		*mCsv 
                << "\t" << row.training[i].nmse
                << "\t" << row.training[i].mse;
	}	

	// dane testowe
	*mCsv 
        << "\t" << row.testing.getNMSE();

	// zmienne wyjściowe
	for(unsigned i=0; i<mConfig->getOutputs().size(); i++) {
        *mCsv
                << "\t" << row.testing[i].nmse
                << "\t" << row.testing[i].mse;
	}	
}
