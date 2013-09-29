TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE += -march=native -mssse3 -fomit-frame-pointer
LIBS += -lboost_unit_test_framework -ltbb

SOURCES += main.cpp \
    async-gost.cpp \
    CryptoRequest.cpp \
    CryptoEngineSlot.cpp \
    CryptoRequestCFBEncrypt.cpp \
    CryptoEngineGeneric.cpp \
    CryptoRequestCFBDecrypt.cpp \
    CryptoRequestECBEncrypt.cpp \
    CryptoRequestImit.cpp \
    CryptoTest.cpp \
    CryptoEngineSSSE3.cpp

HEADERS += \
    CryptoRequest.h \
    CryptoEngineSlot.h \
    CryptoRequestCFBEncrypt.h \
    CryptoEngineGeneric.h \
    CryptoRequestCFBDecrypt.h \
    CryptoRequestECBEncrypt.h \
    async-gost.h \
    CryptoRequestImit.h \
    CryptoEngineSSSE3.h

