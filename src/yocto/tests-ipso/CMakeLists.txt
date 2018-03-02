FILE(GLOB test_srcs test-*.cpp)
ADD_EXECUTABLE(test-ipso main.cpp ${test_srcs})
TARGET_LINK_YOCTO(test-ipso)

FIND_PACKAGE(MPI)

IF(MPI_FOUND)
	INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})
	INCLUDE_DIRECTORIES(../../mpi)

	ADD_EXECUTABLE(test-ipso-par main-par.cpp)
	TARGET_LINK_YOCTO(test-ipso-par mpi)
	TARGET_LINK_LIBRARIES(test-ipso-par ${MPI_LIBRARIES})
ENDIF(MPI_FOUND)


ENABLE_TESTING()
ADD_TEST(ipso     test-ipso ipso)
ADD_TEST(divide   test-ipso divide)

