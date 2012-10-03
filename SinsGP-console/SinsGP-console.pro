TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += \
    stdafx.cpp \
    console.cpp

HEADERS += \
    targetver.h \
    stdafx.h

# prekompilowany nag³ówek
PRECOMPILED_HEADER = stdafx.h

# za³¹czanie biblioteki programu SinsGP
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SinsGP-lib/release/ -lSinsGP-lib -L$$OUT_PWD/../gnuplot++/release/ -lgnuplot++
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SinsGP-lib/debug/ -lSinsGP-lib -L$$OUT_PWD/../gnuplot++/debug/ -lgnuplot++
else:unix: LIBS += -L$$OUT_PWD/../SinsGP-lib/ -lSinsGP-lib -L$$OUT_PWD/../gnuplot++/ -lgnuplot++

INCLUDEPATH += $$PWD/../SinsGP-lib $$PWD/../gnuplot++
DEPENDPATH += $$PWD/../SinsGP-lib $$PWD/../gnuplot++

# biblioteki, które musz¹ byæ poprzednio zbudowane
win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SinsGP-lib/release/libSinsGP-lib.a  $$OUT_PWD/../gnuplot++/release/libgnuplot++.a
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SinsGP-lib/debug/libSinsGP-lib.a $$OUT_PWD/../gnuplot++/debug/libgnuplot++.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../SinsGP-lib/libSinsGP-lib.a $$OUT_PWD/../gnuplot++/libgnuplot++.a

# do³¹czanie bibliotek Open Beagle
win32: LIBS +=  c:/QtSDK/mingw/lib/libbeagle.a c:/QtSDK/mingw/lib/libbeagle-GP.a c:/QtSDK/mingw/lib/libpacc-xml.a c:/QtSDK/mingw/lib/libpacc-util.a c:/QtSDK/mingw/lib/libpacc-math.a
else:unix: LIBS += -lbeagle -lbeagle-GP

# do³¹czenie biblioteki boost
win32:CONFIG(release, debug|release): LIBS += -lboost_filesystem-mgw44-mt-1_51 -lboost_system-mgw44-mt-1_51
else:win32:CONFIG(debug, debug|release): LIBS += -lboost_filesystem-mgw44-mt-d-1_51 -lboost_system-mgw44-mt-d-1_51
else:unix: LIBS += -lboost_filesystem

# skopiowanie pliku wykonywalnego do katalogu g³ównego
win32:CONFIG(release, debug|release): FROM = $$OUT_PWD/release/SinsGP-console.exe
win32:CONFIG(debug, debug|release): FROM = $$OUT_PWD/debug/SinsGP-console.exe
win32:FROM ~= s,/,\\,g
win32:TO = $$OUT_PWD/../../../SinsGP.exe
win32:TO ~= s,/,\\,g
unix: FROM = $$OUT_PWD/console
unix: TO = $$OUT_PWD/../../../SinsGP
win32:CONFIG(release, debug|release): QMAKE_POST_LINK += $$quote(copy $$FROM $$TO)
else:unix: QMAKE_POST_LINK += $$quote(cp $$FROM $$TO)
