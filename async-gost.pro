TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11
LIBS += -lboost_unit_test_framework -lboost_thread -ltbb

SOURCES += main.cpp \
    async-gost.cpp \
    gost-generic.cpp \
    GostGenericEngine.cpp

HEADERS += \
    async-gost.hpp \
    GostGenericEngine.h

