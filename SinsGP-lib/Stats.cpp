#include "stdafx.h"
#include "Stats.h"

using namespace SinsGP;
using namespace boost::filesystem;

Stats::Row::Row(unsigned par, unsigned sig)
    : par(par, 0),
      sig(sig)
{
}

Stats::Stats(boost::filesystem::path path, unsigned par, unsigned sig)
    : mPar(par),
      mSig(sig),
      mParHeaders(par),
      mSigHeaders(sig),
      mCsvPath(path)
{
    Beagle_StackTraceBeginM();
    mCsv = new std::ofstream(mCsvPath.string().c_str());
    if(!mCsv->good())
        throw std::runtime_error("SinsGP::Stats::Stats() Nie udalo sie otworzyc do zapisu pliku " + mCsvPath.string());
    Beagle_StackTraceEndM("SinsGP::Stats::Stats(boost::filesystem::path path, unsigned par, unsigned sig)");
}

Stats::~Stats()
{
    Beagle_StackTraceBeginM();
    mCsv->close();
    delete mCsv;
    Beagle_StackTraceEndM("SinsGP::Stats::~Stats()");
}

void Stats::setParamHeader(unsigned i, std::string header) {
    Beagle_StackTraceBeginM();
    Beagle_AssertM(i < mPar);
    mParHeaders[i] = header;
    Beagle_StackTraceEndM("void SinsGP::Stats::setParamHeader(unsigned i, std::string header)");
}

void Stats::setSignalHeader(unsigned i, std::string header) {
    Beagle_StackTraceBeginM();
    Beagle_AssertM(i < mSig);
    mSigHeaders[i] = header;
    Beagle_StackTraceEndM("void SinsGP::Stats::setSignalHeader(unsigned i, std::string header)");
}

void Stats::addHeaders() {
    Beagle_StackTraceBeginM();
    bool tab = false;
    for(unsigned i=0; i<mPar; ++i) {
        if(tab) *mCsv << "\t";
        else tab = true;
        *mCsv << mParHeaders[i];
    }
    for(unsigned i=0; i<mSig; ++i) {
        if(tab) *mCsv << "\t";
        else tab = true;
        *mCsv << mSigHeaders[i] << " [mean]";
        *mCsv << mSigHeaders[i] << " [RMS]";
        *mCsv << mSigHeaders[i] << " [stdError]";
    }
    *mCsv << std::endl;
    Beagle_StackTraceEndM("void SinsGP::Stats::addHeaders()");
}

void Stats::addRow(const Row& row) {
    Beagle_StackTraceBeginM();
    Beagle_AssertM(row.par.size() == mPar);
    Beagle_AssertM(row.sig.size() == mSig);


    bool tab = false;
    for(unsigned i=0; i<mPar; ++i) {
        if(tab) *mCsv << "\t";
        else tab = true;
        *mCsv << row.par[i];
    }
    for(unsigned i=0; i<mSig; ++i) {
        if(tab) *mCsv << "\t";
        else tab = true;
        *mCsv << row.sig[i].mean;
        *mCsv << row.sig[i].rms;
        *mCsv << row.sig[i].stdErr;
    }

    // nowa linijka
    *mCsv << std::endl;
    Beagle_StackTraceEndM("void SinsGP::Stats::addRow(const Row& row)");
}
