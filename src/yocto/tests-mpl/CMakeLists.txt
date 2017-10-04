SET(keys_inc rsa-keys.inc)
SET(keys_bin rsa-keys.bin)
ADD_CUSTOM_COMMAND(
OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${keys_inc}
COMMAND yocto-file2data ${CMAKE_CURRENT_SOURCE_DIR}/${keys_bin} ${CMAKE_CURRENT_SOURCE_DIR}/${keys_inc}
DEPENDS yocto-file2data ${CMAKE_CURRENT_SOURCE_DIR}/${keys_bin}
)

INCLUDE_DIRECTORIES(../../mk)

FILE(GLOB test_srcs test-*.cpp)
ADD_EXECUTABLE(test-mpl main.cpp ${test_srcs} ${keys_inc})
TARGET_LINK_YOCTO(test-mpl)



ENABLE_TESTING()
ADD_TEST(mem test-mpl  mem)
ADD_TEST(mpn test-mpl  mpn)
ADD_TEST(mpz test-mpl  mpz)
ADD_TEST(mpq test-mpl  mpq)
ADD_TEST(hash test-mpl hash)

FIND_PACKAGE(MPI)

IF(MPI_FOUND)
	INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})
	INCLUDE_DIRECTORIES(../../mpi)

	ADD_EXECUTABLE(test-mpl-par main-par.cpp)
	TARGET_LINK_YOCTO(test-mpl-par mpi)
	TARGET_LINK_LIBRARIES(test-mpl-par ${MPI_LIBRARIES})



ENDIF(MPI_FOUND)

