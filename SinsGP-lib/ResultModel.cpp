#include "stdafx.h"
#include <sstream>
#include <fstream>
#include "SinsGP.h"

using namespace SinsGP;
using namespace Beagle;
using namespace std;
using namespace boost::filesystem;
using namespace PACC::XML;

ResultModel::ResultModel(Config* config, GP::Individual::Handle indi, Status& status)
    : mConfig(config), mIndi(indi), mStatus(&status)
{
	// odczyt rzêdu modelu osobnika
    int order = mIndi->size() + mConfig->getDerivatives().size() + mConfig->getObservables().size() - mConfig->getOutputs().size();;

	// czy osobnik ma odpowiedni¹ liczbê drzew
	if(order > mConfig->getMaxOrder() || order < mConfig->getMinOrder()) {
        std::string e = "SinsGP::ResultModel: Osobnik zawiera " + int2str(mIndi->size()) + " drzew, a powinno byæ od " + int2str((mConfig->getOutputs().size()+mConfig->getMinOrder()-mConfig->getObservables().size()-mConfig->getDerivatives().size())) + " do " + int2str((mConfig->getOutputs().size()+mConfig->getMaxOrder()-mConfig->getObservables().size()-mConfig->getDerivatives().size()));
		throw runtime_error(e);
	}
}

void ResultModel::exec() {
	int outputs = mConfig->getOutputs().size();

    // odczyt rzêdu modelu osobnika
    int order = mIndi->size() + mConfig->getDerivatives().size() + mConfig->getObservables().size() - mConfig->getOutputs().size();

	///////////////////////////////////////////////////////
	// Zapis modelu do pliku
	///////////////////////////////////////////////////////

	// plik do zapisu modelu
    path file = mConfig->getPaths().run / mStatus->getModelPath() / path("model.txt");
    ofstream ofs(file.string().c_str());

	if(!ofs.good())
		throw runtime_error("SinsGP::ResultModel::save(): Nie uda³o siê otworzyæ do zapisu pliku " + file.string());

    // wypisywanie równañ wyjœæ
    int obs = mConfig->getObservables().size();
    std::string outEquations;
    for(int i=0; i<obs; ++i) {
        outEquations += "y" + int2str(i) + " = x" + int2str(mConfig->getObservables(i)) + "\n";
    }
    for(int i=obs; i<outputs; ++i) {
        outEquations += "y" + int2str(i) + " = " + flattenTree((*mIndi)[i-obs]) + "\n";
    }

    // wypisywanie równañ stanu
    for(int i=0, trees=outputs-obs; i<order; ++i) {
		if(mConfig->getContinous()) {
            ofs << "dx" + int2str(i) + "/dt = ";
		} else {
            ofs << "x" + int2str(i) + "(k+1) = ";
		}
        if(mConfig->isDerivative(i)) {
            ofs << "x" + int2str(i-1);
        } else {
            ofs << flattenTree((*mIndi)[trees]);
            ++trees;
        }
        if(!mConfig->getContinous()) {
            ofs << "+x"+int2str(i);
        }
        ofs << endl;
	}
    ofs << outEquations;

	ofs.close();

	////////////////////////////////////////////////////////////
	// Zapis danych wynikowych dla ka¿dego przypadku danych
	////////////////////////////////////////////////////////////

    for(unsigned i=0; i<this->size(); i++) {
		(*this)[i]->exec();
	}
}

std::string ResultModel::flattenTree(Beagle::GP::Tree::Handle tree)
{
	// serializacja drzewa do postaci xml
	stringstream stream(tree->serialize());
	// parsowanie powsta³ego xml
	Document xml(stream);
	// pobieranie taga "genotype"
	Iterator genotype = xml.getFirstDataTag();
	// rekurencyjny odczyt drzewa
	return flattenXML(genotype->getFirstChild());
}

std::string ResultModel::flattenXML(PACC::XML::Node* node) {
	std::string name = node->getValue();
	// dodawanie
	if(name == "ADD") {
		Node* child = node->getFirstChild();
		Node* child2 = node->getLastChild();
		return "(" + flattenXML(child) + "+" + flattenXML(child2) + ")";
	} 
	// odejmowanie
	else if(name == "SUB") {
		Node* child2 = node->getLastChild();
		Node* child = node->getFirstChild();
		return "(" + flattenXML(child) + "-" + flattenXML(child2) + ")";
	} 
	// mno¿enie
	else if(name == "MUL") {
		Node* child2 = node->getLastChild();
		Node* child = node->getFirstChild();
		return "(" + flattenXML(child) + "*" + flattenXML(child2) + ")";
	}
	// dzielenie
	else if(name == "DIV") {
		Node* child2 = node->getLastChild();
		Node* child = node->getFirstChild();
		return "(" + flattenXML(child) + "/" + flattenXML(child2) + ")";
	} 
	// sinus
	else if(name == "SIN") {
		Node* child = node->getFirstChild();
		return "sin(" + flattenXML(child) + ")";
	} 
	// cosinus
	else if(name == "COS") {
		Node* child = node->getFirstChild();
		return "cos(" + flattenXML(child) + ")";
	} 
	// eksponenta
	else if(name == "EXP") {
		Node* child = node->getFirstChild();
		return "exp(" + flattenXML(child) + ")";
	} 
	// logarytm naturalny
	else if(name == "LOG") {
		Node* child = node->getFirstChild();
		return "log(" + flattenXML(child) + ")";
    }
    // bezpieczny pierwiastek
    else if(name == "SSQRT") {
        Node* child = node->getFirstChild();
        std::string flattened = flattenXML(child);
        return "sqrt(abs(" + flattened + "))*sign(" + flattened + ")";
    }
	// sta³a
	else if(name == "E") {
		return node->getAttribute("value");
	}
	// zmienna
	else if(node->getChildCount() == 0) {
		return node->getValue();
	}
	return "";
}
