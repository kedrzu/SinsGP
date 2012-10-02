#include "stdafx.h"

#include <fstream>

#include "SinsGP.h"

using namespace SinsGP;
using namespace Beagle;
using namespace boost::filesystem;
using namespace std;

WriteStatsOp::WriteStatsOp(Config* config, Beagle::string inName)
	: Operator(inName), mConfig(config)
{
}

WriteStatsOp::Stats::Stats(SinsGP::Config *config)
	: mConfig(config)
{

}

void WriteStatsOp::operate(Beagle::Deme &ioDeme, Beagle::Context&)
{
	// szukamy deme w mapie
	map<Deme*, WriteStatsOp::Stats*>::iterator statsIt = mStats.find(&ioDeme);
	
	// odczyt nag³ówków
	if(mHeaders.empty()) {
        for(unsigned i=0; i<ioDeme.getStats()->size(); i++) {
			mHeaders.push_back((*ioDeme.getStats())[i].mId);
		}
	}

	// czy deme jest ju¿ w statystykach?
	if(statsIt == mStats.end()) {
		Beagle::Stats::Handle stats = ioDeme.getStats();		

		// dodanie nowego elementu
		mStats[&ioDeme] = new Stats(mConfig);
	}

	statsIt = mStats.find(&ioDeme);
	// nie uda³o siê drugi raz
	if(statsIt == mStats.end()) {
		throw std::runtime_error("WriteStatsOp::operate(): Nie uda³o siê wczytaæ deme z mapy");	
	} else {
		Beagle::Stats::Handle stats = ioDeme.getStats();
		Row row;
        for(unsigned i=0; i<stats->size(); i++) {
			boost::array<double,4> arr = {{(*stats)[i].mAvg, (*stats)[i].mStd, (*stats)[i].mMax, (*stats)[i].mMin}};
			row.push_back(arr);
		}
		statsIt->second->mData.push_back(row);
	}
}

void WriteStatsOp::exec(Beagle::Vivarium::Handle vivarium, Status &status)
{
    path runPath = mConfig->getPaths().run / status.getRunPath();
    create_directories(runPath);
    for(unsigned i=0; i<vivarium->size(); i++) {
		if(mConfig->getResultStats() & Config::CSV) {
            path csvPath = runPath / path("deme-" + int2str(i) + "-stats.csv");
            ofstream csv(csvPath.string().c_str());

			if(!csv.good())
				throw std::runtime_error("SinsGP::Result::Result() Nie udalo sie otworzyc do zapisu pliku " + csvPath.string());

			// numer generacji
			csv << "generation";

			// reszta nag³ówków
            for(unsigned j=0; j<mHeaders.size(); j++) {
				csv << "\t" << mHeaders[j] << "-avg";
				csv << "\t" << mHeaders[j] << "-std";
				csv << "\t" << mHeaders[j] << "-max";
				csv << "\t" << mHeaders[j] << "-min";
			}

			// wyszukanie statystyk z mapy
			map<Deme*, WriteStatsOp::Stats*>::iterator statsIt = mStats.find(&(*(*vivarium)[i]));
			
			// czy znaleziono
			if(statsIt == mStats.end()) {
				throw std::runtime_error("WriteStatsOp::exec(): Nie znaleziono statystyk podanego deme");
			} else {
				WriteStatsOp::Stats* stats = statsIt->second;

				// iterujemy po zawartoœci statystyk
				int gen = 0;
				for( list<Row>::iterator iter=stats->mData.begin(); iter != stats->mData.end(); ++iter ) {
					// nowa linijka
					csv << endl;
					// numer generacji
					csv << gen;
                    for( unsigned j=0; j<iter->size(); j++) {
						csv << "\t" << (*iter)[j][0];
						csv << "\t" << (*iter)[j][1];
						csv << "\t" << (*iter)[j][2];
						csv << "\t" << (*iter)[j][3];
					}
					gen++;
				}
			}
			csv.close();

		}
	}
}

void WriteStatsOp::clear() {
    mStats.clear();
    mHeaders.clear();
}
