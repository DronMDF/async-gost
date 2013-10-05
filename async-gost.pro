TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE += -march=pentium4 -fomit-frame-pointer
# -mno-avx Позволяет отключить оптимизацию для avx процессоров, а то код получается разный
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
    CryptoRequestNull.cpp \
    CpuSupport.cpp \
    CryptoThread.cpp

HEADERS += \
    CryptoRequest.h \
    CryptoEngineSlot.h \
    CryptoRequestCFBEncrypt.h \
    CryptoEngineGeneric.h \
    CryptoRequestCFBDecrypt.h \
    CryptoRequestECBEncrypt.h \
    async-gost.h \
    CryptoRequestImit.h \
    CryptoEngineSSSE3.h \
    CryptoEngine.h \
    CryptoRequestNull.h \
    CpuSupport.h \
    CryptoThread.h \
    CryptoEngineAVX2.h

SSSE3_SOURCES = CryptoEngineSSSE3.cpp
ssse3.input = SSSE3_SOURCES
ssse3.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_IN_BASE}$${first(QMAKE_EXT_OBJ)}
ssse3.commands = $${QMAKE_CXX} $(CXXFLAGS) -mtune=core2 -mssse3 -flax-vector-conversions $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
ssse3.dependency_type = TYPE_C
ssse3.variable_out = OBJECTS

AVX2_SOURCES = CryptoEngineAVX2.cpp
avx2.input = AVX2_SOURCES
avx2.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_IN_BASE}$${first(QMAKE_EXT_OBJ)}
avx2.commands = $${QMAKE_CXX} $(CXXFLAGS) -mtune=core-avx2 -mavx2 -flax-vector-conversions $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
avx2.dependency_type = TYPE_C
avx2.variable_out = OBJECTS

QMAKE_EXTRA_COMPILERS += ssse3 avx2
OTHER_FILES += $$SSSE3_SOURCES $$AVX2_SOURCES
