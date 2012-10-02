#include "stdafx.h"
#include "StatsCalcFitnessNMSEOp.h"
#include <limits>

using namespace SinsGP;
using namespace Beagle;
using namespace std;

StatsCalcFitnessNMSEOp::StatsCalcFitnessNMSEOp(Config& config, Beagle::string inName)
    : StatsCalculateOp(inName), mConfig(&config)
{
}

void StatsCalcFitnessNMSEOp::calculateStatsDeme(Stats &outStats, Deme &ioDeme, Context &ioContext) const {
    Beagle_StackTraceBeginM();
    outStats.clear();
    outStats.clearItems();
    int outputs = mConfig->getOutputs().size();
    outStats.addItem("processed", ioContext.getProcessedDeme());
    outStats.addItem("total-processed", ioContext.getTotalProcessedDeme());

    if(ioDeme.size() == 0) {
        outStats.setGenerationValues(Beagle::string("deme")+uint2str(ioContext.getDemeIndex()+1),
                                     ioContext.getGeneration(), 0, true);
        outStats.resize(3+outputs*2);
        outStats[0].mId  = "NMSE";
        outStats[0].mAvg = 0.0;
        outStats[0].mStd = 0.0;
        outStats[0].mMax = 0.0;
        outStats[0].mMin = 0.0;

        outStats[1].mId = "treedepth";
        outStats[1].mAvg = 0.0;
        outStats[1].mStd = 0.0;
        outStats[1].mMax = 0.0;
        outStats[1].mMin = 0.0;

        outStats[2].mId = "treesize";
        outStats[2].mAvg = 0.0;
        outStats[2].mStd = 0.0;
        outStats[2].mMax = 0.0;
        outStats[2].mMin = 0.0;

        for(int i=0; i<outputs; ++i) {
            int nr = 3 + i*2;
            outStats[nr].mId = "y" + int2str(i) + "-NMSE";
            outStats[nr].mAvg = 0.0;
            outStats[nr].mStd = 0.0;
            outStats[nr].mMax = 0.0;
            outStats[nr].mMin = 0.0;
            ++nr;
            outStats[nr].mId = "y" + int2str(i) + "-MSE";
            outStats[nr].mAvg = 0.0;
            outStats[nr].mStd = 0.0;
            outStats[nr].mMax = 0.0;
            outStats[nr].mMin = 0.0;
        }

        return;
    }

    const GP::Deme& gpDeme = castObjectT<const GP::Deme&>(ioDeme);
    const FitnessNMSE::Handle firstIndivFitness = castHandleT<FitnessNMSE>(ioDeme[0]->getFitness());


    if(ioDeme.size() == 1) {
        outStats.setGenerationValues(Beagle::string("deme")+uint2str(ioContext.getDemeIndex()+1),
                                     ioContext.getGeneration(), 1, true);

        outStats.resize(3+outputs*2);
        outStats[0].mId  = "NMSE";
        outStats[0].mAvg = firstIndivFitness->getNMSE();
        outStats[0].mStd = 0.0;
        outStats[0].mMax = firstIndivFitness->getNMSE();
        outStats[0].mMin = firstIndivFitness->getNMSE();

        outStats[1].mId = "treedepth";
        outStats[1].mAvg = gpDeme[0]->getMaxTreeDepth();
        outStats[1].mStd = 0.0;
        outStats[1].mMax = outStats[1].mAvg;
        outStats[1].mMin = outStats[1].mAvg;

        outStats[2].mId = "treesize";
        outStats[2].mAvg = gpDeme[0]->getTotalNodes();
        outStats[2].mStd = 0.0;
        outStats[2].mMax = outStats[2].mAvg;
        outStats[2].mMin = outStats[2].mAvg;

        for(int i=0; i<outputs; ++i) {
            int nr = 3 + i*2;
            double nmse = (*firstIndivFitness)[i].nmse;
            double mse = (*firstIndivFitness)[i].mse;
            outStats[nr].mId = "y" + int2str(i) + "-NMSE";
            outStats[nr].mAvg = nmse;
            outStats[nr].mStd = 0.0;
            outStats[nr].mMax = nmse;
            outStats[nr].mMin = nmse;
            ++nr;
            outStats[nr].mId = "y" + int2str(i) + "-MSE";
            outStats[nr].mAvg = mse;
            outStats[nr].mStd = 0.0;
            outStats[nr].mMax = mse;
            outStats[nr].mMin = mse;
        }
        return;
    }

    double sum = firstIndivFitness->getNMSE();
    double pow2sum = pow2Of<double>(sum);
    double max = sum;
    double min = sum;

    std::vector<double> sumNMSE(outputs, 0);
    std::vector<double> pow2sumNMSE(outputs, 0);
    std::vector<double> maxNMSE(outputs, 0);
    std::vector<double> minNMSE(outputs, 0);

    std::vector<double> sumMSE(outputs, 0);
    std::vector<double> pow2sumMSE(outputs, 0);
    std::vector<double> maxMSE(outputs, 0);
    std::vector<double> minMSE(outputs, 0);

    for(int i=0; i<outputs; ++i) {
        double nmse = (*firstIndivFitness)[i].nmse;
        double mse = (*firstIndivFitness)[i].mse;

        sumNMSE[i]      = nmse;
        pow2sumNMSE[i]  = pow2Of<double>(nmse);
        maxNMSE[i]      = nmse;
        minNMSE[i]      = nmse;

        sumMSE[i]       = mse;
        pow2sumMSE[i]   = pow2Of<double>(mse);
        maxMSE[i]       = mse;
        minMSE[i]       = mse;
    }

    unsigned int lMaxDepth = gpDeme[0]->getMaxTreeDepth();
    unsigned int lMinDepth = lMaxDepth;
    double lSumDepth       = (double)lMaxDepth;
    double lPow2SumDepth   = pow2Of<double>(lSumDepth);
    unsigned int lMaxSize  = gpDeme[0]->getTotalNodes();
    unsigned int lMinSize  = lMaxSize;
    double lSumSize        = (double)lMaxSize;
    double lPow2SumSize    = pow2Of<double>(lSumSize);

    unsigned indivs = 0;

    for(unsigned int i=1; i<ioDeme.size(); i++) {
        const FitnessNMSE::Handle indivFitness = castHandleT<FitnessNMSE>(ioDeme[i]->getFitness());
        double tNmse = indivFitness->getNMSE();

        if(tNmse != tNmse || tNmse == numeric_limits<double>::infinity()) {
            continue;
        }
        ++indivs;

        sum     += tNmse;
        pow2sum += pow2Of<double>(tNmse);
        max     =  maxOf<double>(max, tNmse);
        min     =  minOf<double>(min, tNmse);

        for(int j=0; j<outputs; ++j) {
            double nmse = (*indivFitness)[i].nmse;
            double mse = (*indivFitness)[i].mse;

            sumNMSE[j] += nmse;
            pow2sumNMSE[j] += pow2Of<double>(nmse);
            maxNMSE[j] = maxOf<double>(maxNMSE[j], nmse);
            minNMSE[j] = minOf<double>(minNMSE[j], nmse);

            sumMSE[j] += mse;
            pow2sumMSE[j] += pow2Of<double>(mse);
            maxMSE[j] = maxOf<double>(maxMSE[j], mse);
            minMSE[j] = minOf<double>(minMSE[j], mse);
        }

        unsigned int lTmpDepth = gpDeme[i]->getMaxTreeDepth();
        lSumDepth     += (double)lTmpDepth;
        lPow2SumDepth += pow2Of<double>((double)lTmpDepth);
        lMaxDepth     =  maxOf(lMaxDepth, lTmpDepth);
        lMinDepth     =  minOf(lMinDepth, lTmpDepth);

        unsigned int lTmpSize = gpDeme[i]->getTotalNodes();
        lSumSize     += (double)lTmpSize;
        lPow2SumSize += pow2Of<double>((double)lTmpSize);
        lMaxSize     =  maxOf(lMaxSize, lTmpSize);
        lMinSize     =  minOf(lMinSize, lTmpSize);
    }


    float lDepthStdError =
            (float)(lPow2SumDepth - (pow2Of<double>(lSumDepth) / indivs)) / (indivs - 1);
    lDepthStdError       = sqrt(lDepthStdError);

    float lSizeStdError =
            (float)(lPow2SumSize - (pow2Of<double>(lSumSize) / indivs)) / (indivs - 1);
    lSizeStdError       = sqrt(lSizeStdError);

    outStats.setGenerationValues(Beagle::string("deme")+uint2str(ioContext.getDemeIndex()+1),
                                 ioContext.getGeneration(),
                                 ioDeme.size(),
                                 true);

    outStats.resize(3+outputs*2);
    outStats[0].mId  = "NMSE";
    outStats[0].mAvg = sum / indivs;
    outStats[0].mStd = sqrt((pow2sum - (pow2Of<double>(sum)/indivs)) / (indivs-1));
    outStats[0].mMax = max;
    outStats[0].mMin = min;

    outStats[1].mId = "treedepth";
    outStats[1].mAvg = (float)(lSumDepth / indivs);
    outStats[1].mStd = lDepthStdError;
    outStats[1].mMax = (float)lMaxDepth;
    outStats[1].mMin = (float)lMinDepth;

    outStats[2].mId = "treesize";
    outStats[2].mAvg = (float)(lSumSize / indivs);
    outStats[2].mStd = lSizeStdError;
    outStats[2].mMax = (float)lMaxSize;
    outStats[2].mMin = (float)lMinSize;

    for(int j=0; j<outputs; ++j) {
        int nr = 3 + j*2;
        outStats[nr].mId = "y" + int2str(j) + "-NMSE";
        outStats[nr].mAvg = sumNMSE[j] / indivs;
        outStats[nr].mStd = sqrt((pow2sumNMSE[j] - (pow2Of<double>(sumNMSE[j])/indivs)) / (indivs-1));
        outStats[nr].mMax = maxNMSE[j];
        outStats[nr].mMin = minNMSE[j];
        ++nr;
        outStats[nr].mId = "y" + int2str(j) + "-MSE";
        outStats[nr].mAvg = sumMSE[j] / indivs;
        outStats[nr].mStd = sqrt((pow2sumMSE[j] - (pow2Of<double>(sumMSE[j])/indivs)) / (indivs-1));
        outStats[nr].mMax = maxMSE[j];
        outStats[nr].mMin = minMSE[j];
    }

    Beagle_StackTraceEndM("void SinsGP::StatsCalcFitnessNMSEOp::calculateStatsDeme(Beagle::Stats& outStats, Beagle::Deme& ioDeme, Beagle::Context& ioContext) const");
}
