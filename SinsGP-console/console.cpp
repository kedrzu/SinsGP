#include "stdafx.h"

#include "SinsGP.h"

#include <iostream>
#include <fstream>
#include <cstdlib> // do wywoania system() 

using namespace std;
using namespace Beagle;
using namespace SinsGP;
using namespace boost::filesystem;



void doPause()
{
    cout << "Press [ Enter ] to continue...";
    cin.clear(); // Make sure the buffers are clear, you may want to do cin.flush() as well but I am not sure if both are needed.
    cin.ignore();
}

int main(int argc, char *argv[]) {
    try {

        /////////////////////////////////////////////////////////////////////////////////////
        // WCZYTANIE PLIKU KONFIGURACJI
        /////////////////////////////////////////////////////////////////////////////////////


        // musi by podana cieka dostpu do pliku konfiguracyjnego
        if(argc < 2) {
            cout << "Podaj sciezke dostepu do plikow problemu";
            return 1;
        }

        // wczytywanie konfiguracji z podanego pliku
        Config config(argc, argv);

        // zmienna zawierajca cieki dostpu do plikw
        const Config::Paths& paths = config.getPaths();


        cout << endl << "   Zmienne wejsciowe:  ";
        for(unsigned i=0; i<config.getInputs().size(); i++) {
            cout << config.getInputs(i)->getName() << " ";
        }
        cout << endl << "   Zmienne wyjsciowe:  ";
        for(unsigned i=0; i<config.getOutputs().size(); i++) {
            cout << config.getOutputs(i) << " ";
        }
        cout << endl << endl;

        /////////////////////////////////////////////////////////////////////////////////////
        // WCZYTANIE DANYCH UCZCYCH Z PLIKW
        /////////////////////////////////////////////////////////////////////////////////////

        cout << "Wczytywanie danych uczacych:" << endl; // info na konsoli
        vector<path> dataFiles = paths.getDataFiles(); // pobranie listy plikw
        Data::Bag data;
        for(unsigned i=0; i<dataFiles.size(); i++) {
            cout << dataFiles[i] << " ";
            // prba wczytania pliku, jeeli si nie powiedzie zgaszany jest wyjtek
            try {
                data.push_back(new Data(config, dataFiles[i]));
                cout << "OK" << endl;
            } catch(exception& e) {
                cout << e.what() << endl;
            }
        }
        // jezeli nie udalo sie wczytac zadnego pliku, nie mozna kontynuowac
        if(data.size() == 0) {
            cerr << endl << "Nie mozna kontynuowac, nie znaleziono danych uczacych w folderze " << paths.data << endl;
            return 1;
        }
        cout << endl << endl;

        /////////////////////////////////////////////////////////////////////////////////////
        // WCZYTANIE DANYCH TESTOWYCH Z PLIKW
        /////////////////////////////////////////////////////////////////////////////////////

        cout << "Wczytywanie danych testowych:" << endl; // info
        vector<path> testFiles = paths.getTestFiles(); // pobranie listy plikw
        Data::Bag test;
        for(unsigned i=0; i<testFiles.size(); i++) {
            cout << testFiles[i] << " ";
            // prba wczytania pliku, jeeli si nie powiedzie zgaszany jest wyjtek
            try {
                test.push_back(new Data(config, testFiles[i]));
                cout << "OK" << endl;
            } catch(exception& e) {
                cout << e.what() << endl;
            }
        }
        // nawet jeeli nie udalo si wczyta adnych plikw testowych mozna kontynuowaæ uczenie
        if(test.size() == 0) {
            cout << "Nie znaleziono danych testowych w folderze " << paths.test << endl;
        }
        cout << endl << endl;

        // "Wcinij Enter..."
        //doPause();

        // Obiekt do zapisu statystycznych wynikw
        SinsGP::Stats* stats = new SinsGP::Stats(config.getPaths().run / path("result-stats.csv"), 1, 1);

        /////////////////////////////////////////////////////////////////////////////////////
        // KONFIGURACJA ALGORYTMU GENETYCZNEGO
        /////////////////////////////////////////////////////////////////////////////////////

        // okrelenie zestawu prymityww
        GP::PrimitiveSet::Handle set = new GP::PrimitiveSet;
        set->insert(new GP::Add); // dodawanie
        set->insert(new GP::Subtract); // odejmowanie
        set->insert(new GP::Multiply); // mnoenie
        set->insert(new GP::Divide); // dzielenie (bezpieczne)
        set->insert(new GP::Sin); // sinus
        set->insert(new GP::Cos); // cosinus
        set->insert(new GP::Exp); // e^x
        set->insert(new GP::Log); // logarytm naturalny (bezpieczny)
        set->insert(new SinsGP::SignedSqrt); // bezpieczny pierwiastek kwadratowy
        set->insert(new GP::EphemeralDouble); // sta³a liczbowa
        set->insert(new EphemeralDoubleWide); // sta³a liczbowa

        // status procesu optymalizacji
        SinsGP::Status status;

        // licznik uruchomieñ
        while(status.getRun()<config.getRuns() || config.getRuns() == 0) {
            status.nextRun();

            // system
            // musi byc recznie ustawiony seed, poniewaz z jakiegos powodu automatyczne
            // ustawianie seeda na podstawie czasu systemowego nie dziala
            PACC::Date date;
            Beagle::GP::Context::Alloc::Handle contextAllocator = new Beagle::GP::Context::Alloc();
            Beagle::Randomizer::Handle randomizer = new Beagle::Randomizer(date.getTimeInSeconds());
            Beagle::Register::Handle systemRegister = new Beagle::Register();
            Beagle::LoggerXML::Handle logger = new Beagle::LoggerXML();
            Beagle::GP::ModuleVectorComponent::Handle moduleVectorComponent = new Beagle::GP::ModuleVectorComponent();
            GP::System::Handle gpSystem = new GP::System(set, contextAllocator, randomizer, systemRegister, logger, moduleVectorComponent);

            // operator obliczania funkcji przystosowania
            DynObjEvalOp::Handle evalOp = new DynObjEvalOp(config, data);
            // operator do testowania modeli i tworzenia plików wynikowych
            EvalResultsOp::Handle evalResults = new EvalResultsOp(config, data, test, stats);
            // evolver
            SinsGP::Evolver::Handle evolver = new SinsGP::Evolver(evalOp, &config, status);
            // operator do obliczania statystyk uczenia
            //StatsCalcFitnessNMSEOp::Handle statsCalc = new StatsCalcFitnessNMSEOp(config, "StatsCalcFitnessOp");
            //StatsCalcFitnessSimpleOp::Handle statsCalc = new StatsCalcFitnessSimpleOp("StatsCalcFitnessOp");
            //evolver->addOperator(statsCalc);
            // obiekt do zapisu wynikw statystycznych w trakcie uczenia
            WriteStatsOp::Handle statsOp = new WriteStatsOp(&config);
            evolver->addOperator(statsOp);
            // inicjalizacja evolvera
            evolver->initialize(gpSystem, paths.beagleConfig.string());

            /////////////////////////////////////////////////////////////////////////////////////
            // KONFIGURACJA LICZBY ZMIENNYCH MODELU
            /////////////////////////////////////////////////////////////////////////////////////

            // Konfiguracja listy zmiennych wejciowych
            for(unsigned i=0; i<config.getInputs().size(); i++) {
                for(unsigned j=0; j<gpSystem->getPrimitiveSuperSet().size(); j++) {
                    gpSystem->getPrimitiveSuperSet().getPrimitSetBag()[j]->insert(config.getInputs(i));
                }
            }
            // Konfiguracja listy zmiennych stanu
            for(unsigned i=0; i<config.getMaxOrder(); i++) {
                for(unsigned j=0; j<gpSystem->getPrimitiveSuperSet().size(); j++) {
                    gpSystem->getPrimitiveSuperSet().getPrimitSetBag()[j]->insert(config.getStateVar(i));
                }
            }

            // vivarium
            GP::Vivarium::Handle vivarium = new GP::Vivarium(new GP::Tree::Alloc, new FitnessCorelation::Alloc);

            // uruchomienie optymalizacji genetycznej
            evolver->evolve(vivarium);

            // zapis statystyk uczenia do plikow
            statsOp->exec(vivarium, status);

            // czyszczenie statystyk
            statsOp->clear();

            // pliki do skopiowania z temp do run (zrodlo i cel)
            vector<path> toCopyFrom;
            vector<path> toCopyTo;

            // string do uruchomienia rasterizera
            std::string rasterize = "java -jar \"" + paths.rasterizer.string() + "\"";

            // iteracja po najlepszych osobnikach
            for(unsigned j=0; j<vivarium->getHallOfFame().size(); ++j) {
                status.nextModel();

                // osobnik
                Beagle::GP::Individual::Handle indi = Beagle::castHandleT<Beagle::GP::Individual>(vivarium->getHallOfFame()[j].mIndividual);

                // utworzenie folderu z wynikami osobnika
                path temp = paths.temp / status.getModelPath();
                create_directories(temp);
                path run = paths.run / status.getModelPath();
                create_directories(run);

                // obiekt do zapisu wynikw uczenia osobnika
                ResultModel resModel(&config, indi, status);

                // wygenerowanie wynikow
                evalResults->logResults(resModel, gpSystem);

                // zapis wynikow
                resModel.exec();

                // cieka do pliku wyjciowego xml
                path xmlTemp = temp / path("tree-xml.xml");

                // cieka do pliku wyjciowego svg
                path svgTemp = temp / path("tree-svg.svg");

                //zapisu pliku xml
                if(config.getResultIndModel() & (Config::XML | Config::SVG | Config::PNG)) {
                    ofstream ofs(xmlTemp.string().c_str());
                    PACC::XML::Streamer streamer(ofs);
                    vivarium->getHallOfFame()[j].mIndividual->write(streamer);
                    ofs.close();
                }

                // wykonanie komendy konwersji xml -> svg przy pomocy programu tree2svg
                if(config.getResultIndModel() & (Config::SVG | Config::PNG)) {
                    std::string tree2svg = paths.tree2svg.string() + " \"" + xmlTemp.string() + "\" \"" + svgTemp.string() + "\"";
                    cout << endl << endl << tree2svg << endl << endl;
                    system(tree2svg.c_str());
                }

                // wygenerowanie komendy konwersji SVG -> PNG
                if(config.getResultIndModel() & Config::PNG) {
                    rasterize += " \"" + svgTemp.string() + "\"";
                    // cieki dostpu
                    path pngTemp = temp / path("tree-svg.png");
                    path png = run / path("tree-png.png");
                    // przeniesienie pliku z folderu temp
                    toCopyFrom.push_back(pngTemp);
                    toCopyTo.push_back(png);
                }

                // przeniesienie XML z temp
                if(config.getResultIndModel() & Config::XML) {
                    path xml = run / path("tree-xml.xml");
                    // przeniesienie pliku z folderu temp
                    toCopyFrom.push_back(xmlTemp);
                    toCopyTo.push_back(xml);
                }

                // przeniesienie SVG z temp
                if(config.getResultIndModel() & Config::SVG) {
                    path svg = run / path("tree-svg.svg");
                    // przeniesienie pliku z folderu temp
                    toCopyFrom.push_back(svgTemp);
                    toCopyTo.push_back(svg);
                }

            }
            // wykonanie komendy konwersji SVG->PNG
            if(config.getResultIndModel() & Config::PNG) {
                system(rasterize.c_str());
            }
            // skopiowanie plikw z temp do run
            for(unsigned j=0; j<toCopyFrom.size(); j++) {
                boost::filesystem::rename(toCopyFrom[j], toCopyTo[j]);
            }
        }
    }
    catch(Exception& inException) {
        cerr << inException.what();
        inException.terminate();
        return 1;
    }
    catch(exception& inException) {
        cerr << "[STOP] Wystapily bledy:" << endl;
        cerr << inException.what() << endl << flush;
        return 1;
    }
    catch(...) {
        cerr << "Unknown exception catched!" << endl << flush;
        return 1;
    }
    return 0;
}

