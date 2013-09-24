TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE += -march=native -fomit-frame-pointer
LIBS += -lboost_unit_test_framework -lboost_thread -ltbb

SOURCES += main.cpp \
    async-gost.cpp \
    GostGenericEngine.cpp \
    CryptoRequest.cpp \
    CryptoEngineSlot.cpp

HEADERS += \
    async-gost.hpp \
    GostGenericEngine.h \
    CryptoRequest.h \
    CryptoEngineSlot.h

