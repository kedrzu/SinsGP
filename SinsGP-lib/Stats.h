#pragma once
#include "SinsGP_Signal.h"
#include <string>
#include "stdafx.h"

namespace SinsGP {
	class Stats
	{
	public:

        struct Row {
            Row(unsigned par, unsigned sig);
            std::vector<double> par;
            std::vector<Signal::Stats> sig;
        };

        Stats(boost::filesystem::path path, unsigned par, unsigned sig);
        ~Stats(void);
        void setParamHeader(unsigned i, std::string header);
        void setSignalHeader(unsigned i, std::string header);
        void addHeaders();
        void addRow(const Row& row);
	private:
        unsigned mPar;
        unsigned mSig;
        std::vector<std::string> mParHeaders;
        std::vector<std::string> mSigHeaders;
		std::ofstream* mCsv; //!< Stream do utworzenia pliku csv
        boost::filesystem::path mCsvPath; //!< Ścieżka do csv
	};
}
