#include "stdafx.h"
#include "SinsGP.h"

using namespace SinsGP;
using namespace std;

ResultRow::ResultRow(void)
{
}

ResultRow::ResultRow(int inputs, int outputs, int order)
    : DataRow(inputs, outputs), x(order), yModel(outputs), e(outputs)
{
}

ResultRow::~ResultRow(void)
{
}

ostream& operator <<(ostream &out, const ResultRow &r) 
{
	// czas
	out << r.t;
	// wejœcia
	for(unsigned i=0; i<r.u.size(); i++) {
		out << "\t" << r.u[i];	
	}
	// zmienne stanu
	for(unsigned i=0; i<r.x.size(); i++) {
		out << "\t" << r.x[i];
	}
	// wyjœcia
	for(unsigned i=0; i<r.y.size(); i++) {
		out << "\t" << r.y[i] << "\t" << r.yModel[i] << "\t" << r.e[i];
	}

	return out;
}

string ResultRow::headers() 
{
	// czas
	std::string res = "time";
	// wejœcia
	for(unsigned i=0; i<u.size(); i++) {
		res += "\tu" + Beagle::int2str(i);	
	}
	// zmienne stanu
	for(unsigned i=0; i<x.size(); i++) {
		res += "\tx" + Beagle::int2str(i);
	}
	// wyjœcia
	for(unsigned i=0; i<y.size(); i++) {
		res += "\ty" + Beagle::int2str(i);
		res += "\ty" + Beagle::int2str(i) + "-predicted";
		res += "\ty" + Beagle::int2str(i) + "-error";
	}

	return res;
}

string ResultRow::string()
{
	// czas
	std::string out = Beagle::dbl2str(t);
	// wejœcia
	for(unsigned i=0; i<u.size(); i++) {
		out += "\t" + Beagle::dbl2str(u[i]);	
	}
	// zmienne stanu
	for(unsigned i=0; i<x.size(); i++) {
		out += "\t" + Beagle::dbl2str(x[i]);
	}
	// wyjœcia
	for(unsigned i=0; i<y.size(); i++) {
		out += "\t" + Beagle::dbl2str(y[i]) + "\t" + Beagle::dbl2str(yModel[i]) + "\t" + Beagle::dbl2str(e[i]);
	}

	return out;
}
