#include "stdafx.h"
#include "SinsGP.h"

using namespace SinsGP;
using namespace std;

DataRow::DataRow(void)
{
}

DataRow::DataRow(int inputs, int outputs) : u(inputs), y(outputs)
{
}

DataRow::~DataRow(void)
{
}

std::ostream& operator << (std::ostream& out, const DataRow& r) 
{
	// czas
	out << r.t;
	// wej�cia
	for(unsigned i=0; i<r.u.size(); i++) {
		out << "\t" << r.u[i];	
	}
	// wyj�cia
	for(unsigned i=0; i<r.y.size(); i++) {
		out << "\t" << r.y[i];	
	}

	return out;
}

string DataRow::headers()
{
	// czas
	std::string res = "time";
	// wej�cia
	for(unsigned i=0; i<u.size(); i++) {
		res += "\tu" + Beagle::int2str(i);	
	}
	// wyj�cia
	for(unsigned i=0; i<y.size(); i++) {
		res += "\ty" + Beagle::int2str(i);
	}
	return res;
}

string DataRow::string()
{
	// czas
	std::string out = Beagle::dbl2str(t);
	// wej�cia
	for(unsigned i=0; i<u.size(); i++) {
		out += "\t" + Beagle::dbl2str(u[i]);	
	}
	// wyj�cia
	for(unsigned i=0; i<y.size(); i++) {
		out += "\t" + Beagle::dbl2str(y[i]);	
	}

	return out;
}