TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11
LIBS += -lboost_unit_test_framework

SOURCES += main.cpp

