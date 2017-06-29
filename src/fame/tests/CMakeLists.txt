FILE(GLOB test_srcs test-*.cpp *.hpp)
ADD_EXECUTABLE(test-fame main.cpp ${test_srcs})
TARGET_LINK_YOCTO( test-fame mk gfx fame png z )

FIND_PACKAGE(MPI)

IF(MPI_FOUND)
    INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})
	ADD_EXECUTABLE(test-fame-par main-par.cpp)
	TARGET_LINK_YOCTO( test-fame-par mk gfx fame png z mpi)
    TARGET_LINK_LIBRARIES( test-fame-par ${MPI_LIBRARIES})
ENDIF()

ENABLE_TESTING()
ADD_TEST(Types    test-fame types)
ADD_TEST(Layout   test-fame layout)
ADD_TEST(Split    test-fame split)
ADD_TEST(Domain   test-fame domain)
ADD_TEST(Layouts  test-fame layouts)
ADD_TEST(Field    test-fame field)
ADD_TEST(RMesh    test-fame rmesh)
ADD_TEST(CMesh    test-fame cmesh)
 
