#include "stdafx.h"
#include <fstream>
#include "gnuplotpp.h"
#include "SinsGP.h"

using namespace SinsGP;
using namespace boost::filesystem;
using namespace gnuplotpp::detail;

Result::Result(Config& config, int order, Data& data, Status &status, bool test)
    : mConfig(config), mDataFile(data.getFile().filename()), mData(data), mTest(test),
      mOrder(order), mStatus(&status)

{
}

Result::~Result(void)
{
}

void Result::exec() 
{
	int flags = mTest ? mConfig.getResultIndTest() : mConfig.getResultIndData();
    path run = mConfig.getPaths().run / mStatus->getModelPath();
    path temp = mConfig.getPaths().temp / mStatus->getModelPath();
    create_directories(run);
    create_directories(temp);

	// utworzenie pliku csv
	if(flags & Config::CSV) {
        mCsvPath = run / path(mDataFile.filename().string() + "-csv.csv");
		
		// brak danych do zapisu
		if(this->size() == 0) 
            throw runtime_error("SinsGP::Result::exec() Brak danych do zapisania w pliku" + mCsvPath.string());
		 
		// stream
        ofstream csv(mCsvPath.string().c_str());

		if(!csv.good())
			throw std::runtime_error("SinsGP::Result::Result() Nie udalo sie otworzyc do zapisu pliku " + mCsvPath.string());

		// nagłówki pliku
		csv << (*this)[0].headers();

		// zapisujemy do pliku
		for(unsigned i=0; i<this->size(); i++) {
			csv << endl << (*this)[i].string();
		}
		csv.close();	
	}
	if(flags & Config::PLOT) {
        mPlotPath = temp / path(mDataFile.filename().string() + "-plot.csv");

		// brak danych do zapisu
		if(this->size() == 0) 
			throw runtime_error("SinsGP::Result::exec() Brak danych do zapisania w pliku" + mCsvPath.string());
		 
		// stream
        ofstream plot(mPlotPath.string().c_str());

		if(!plot.good())
			throw std::runtime_error("SinsGP::Result::Result() Nie udalo sie otworzyc do zapisu pliku z danymi do wykresu " + mPlotPath.string());

		// zapisujemy do pliku
		for(unsigned i=0; i<this->size(); i++) {
			plot << endl << (*this)[i].string();
		}
		plot.close();

		for( unsigned i=0; i<mConfig.getOutputs().size(); i++ ){
			singleplot g;

			// cerr << g.set().str() << endl;
			// exit(1);

            std::string title = "\"Run: " + Beagle::int2str(mStatus->getRun()) + " Model: " + Beagle::int2str(mStatus->getModel()) + " File: " + (mTest ? "test" : "data") + "/" + mDataFile.string() + " Variable: y" + Beagle::int2str(i) + "\"";
            path plotfile = run / path(mDataFile.string() + "-y" + Beagle::int2str(i) + "-plot.png");
			g.set() << gpset( "term", "png size 1000,500" ) << endl 
				<< gpset("grid")
				<< gpset("title", title) << endl
				<< gpset("output", "'" + plotfile.string() + "'" ) << endl;

			g.x.range(0, (*this)[this->size()-1].t);

            int varNumber = 2+mConfig.getInputs().size()+mOrder+i*3;
            g.plotfile(mPlotPath.string(), 1, varNumber) << " t 'measured'" << " with l lw 2.5 lc rgb '#33ccff'";
			g.plotfile(mPlotPath.string(), 1, varNumber+1) << " t 'predicted'" << " with l lw 1 lc rgb '#000099'";
			g.plotfile(mPlotPath.string(), 1, varNumber+2) << " t 'error'" << " with l lw 1 lc rgb '#ff0000'";

            path gpFile = temp / path((mTest ? "test" : "data") + mDataFile.string() + "-y" + Beagle::int2str(i) + "-gp.gp");
            g.save( gpFile.string());
			string cmd = "gnuplot \"" + gpFile.string() + "\"";
			system(cmd.c_str());
		}
	}
}
