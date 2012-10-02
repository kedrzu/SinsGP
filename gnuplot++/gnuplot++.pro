TEMPLATE = lib
CONFIG += console warn_off staticlib
CONFIG -= qt

SOURCES += \
    process.c \
    gp_plot.cc \
    gp_components.cc \
    gp_base.cc

HEADERS += \
    process.h \
    mkgnu.h \
    gp_traits.h \
    gp_subplot.h \
    gp_plot.h \
    gp_components.h \
    gp_base.h \
    gnuplotpp.h

