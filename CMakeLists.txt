PROJECT(async-gost)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE(cmake/FindTBB.cmake)

#SET(Boost_USE_STATIC_LIBS ON)
#SET(Boost_USE_MULTITHREADED ON)
#SET(Boost_USE_STATIC_RUNTIME OFF)

FIND_PACKAGE(Boost 1.48.0 REQUIRED COMPONENTS unit_test_framework REQUIRED)

SET(SOURCES
	CryptoEngineGeneric.cpp
	CryptoEngineSSSE3.cpp
	CryptoRequestCFBEncrypt.cpp
	CryptoRequestECBEncrypt.cpp
	CryptoRequestNull.cpp
	CryptoEngine.cpp
	CryptoEngineSlot.cpp
	CryptoRequestCFBDecrypt.cpp
	CryptoRequest.cpp
	CryptoRequestImit.cpp
	CryptoTest.cpp
	async-gost.cpp
	main.cpp
)

SET(CMAKE_CXX_FLAGS
	"-std=c++0x -march=native -flax-vector-conversions ${CMAKE_CXX_FLAGS}")

ADD_EXECUTABLE(main ${SOURCES})

TARGET_LINK_LIBRARIES(
	main
	${TBB_LIBRARIES}
	${Boost_LIBRARIES}
)