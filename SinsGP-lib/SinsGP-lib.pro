TEMPLATE = lib
CONFIG += console staticlib
CONFIG -= qt

SOURCES += \
    WriteStatsOp.cpp \
    stdafx.cpp \
    Stats.cpp \
    ResultRow.cpp \
    ResultModel.cpp \
    Result.cpp \
    Evolver.cpp \
    DynObjEvalOp.cpp \
    DiscreteModel.cpp \
    DataRow.cpp \
    Data.cpp \
    ContinousModel.cpp \
    Config.cpp \
    ContinousObserver.cpp \
    ContinousLogger.cpp \
    DiscreteLogger.cpp \
    SignedSqrt.cpp \
    ContinousModelTesting.cpp \
    Status.cpp \
    MilestoneWriteOp.cpp \
    FitnessNMSE.cpp \
    StatsCalcFitnessNMSEOp.cpp \
    EvalResultsOp.cpp \
    ContinousModelConstStep.cpp

HEADERS += \
    WriteStatsOp.h \
    stdafx.h \
    Stats.h \
    ResultRow.h \
    ResultModel.h \
    Result.h \
    SinsGP.h \
    Evolver.h \
    DynObjEvalOp.h \
    DiscreteModel.h \
    DataRow.h \
    Data.h \
    ContinousModel.h \
    Config.h \
    ContinousObserver.h \
    ContinousLogger.h \
    DiscreteLogger.h \
    SignedSqrt.h \
    ContinousModelTesting.h \
    Status.h \
    MilestoneWriteOp.h \
    FitnessNMSE.h \
    StatsCalcFitnessNMSEOp.h \
    EvalResultsOp.h \
    ContinousModelConstStep.h

#PRECOMPILED_HEADER = stdafx.h

INCLUDEPATH += $$PWD/../gnuplot++
