TEMPLATE = lib
CONFIG += console staticlib
CONFIG -= qt

SOURCES += \
    WriteStatsOp.cpp \
    stdafx.cpp \
    Stats.cpp \
    ResultModel.cpp \
    Result.cpp \
    Evolver.cpp \
    DynObjEvalOp.cpp \
    Data.cpp \
    ContinousModel.cpp \
    Config.cpp \
    SignedSqrt.cpp \
    Status.cpp \
    MilestoneWriteOp.cpp \
    FitnessNMSE.cpp \
    StatsCalcFitnessNMSEOp.cpp \
    EvalResultsOp.cpp \
    EphemeralDoubleWide.cpp \
    SinsGP_Signal.cpp \
    FitnessCorelation.cpp

HEADERS += \
    WriteStatsOp.h \
    stdafx.h \
    Stats.h \
    ResultModel.h \
    Result.h \
    SinsGP.h \
    Evolver.h \
    DynObjEvalOp.h \
    Data.h \
    ContinousModel.h \
    Config.h \
    SignedSqrt.h \
    Status.h \
    MilestoneWriteOp.h \
    FitnessNMSE.h \
    StatsCalcFitnessNMSEOp.h \
    EvalResultsOp.h \
    SinsGP_Signal.h \
    EphemeralDoubleWide.h \
    Fitness.h \
    FitnessCorelation.h

PRECOMPILED_HEADER = stdafx.h

INCLUDEPATH += $$PWD/../gnuplot++
