FILE( GLOB top_decl yocto/*.hpp)
FILE( GLOB top_impl yocto/*.cpp)
SET( top_srcs ${top_decl} ${top_impl} )
INSTALL( FILES ${top_decl} DESTINATION include/yocto )

FILE( GLOB type_decl yocto/type/*.hpp)
FILE( GLOB type_impl yocto/type/*.cpp)
SET( type_srcs ${type_decl} ${type_impl} )
INSTALL( FILES ${type_decl} DESTINATION include/yocto/type )
SOURCE_GROUP( type FILES ${type_srcs} )

FILE( GLOB ptr_decl yocto/ptr/*.hpp)
FILE( GLOB ptr_impl yocto/ptr/*.cpp)
SET( ptr_srcs ${ptr_decl} ${ptr_impl} )
INSTALL( FILES ${ptr_decl} DESTINATION include/yocto/ptr )
SOURCE_GROUP( ptr FILES ${ptr_srcs} )


FILE( GLOB sys_decl yocto/sys/*.hpp)
FILE( GLOB sys_impl yocto/sys/*.cpp)
SET( sys_srcs ${sys_decl} ${sys_impl} )
INSTALL( FILES ${sys_decl} DESTINATION include/yocto/sys )
SOURCE_GROUP( sys FILES ${sys_srcs} )

FILE( GLOB code_decl yocto/code/*.hpp)
FILE( GLOB code_impl yocto/code/*.cpp)
SET( code_srcs ${code_decl} ${code_impl})
INSTALL( FILES ${code_decl} DESTINATION include/yocto/code )
SOURCE_GROUP( code FILES ${code_srcs} )

FILE( GLOB sort_decl yocto/sort/*.hpp)
FILE( GLOB sort_impl yocto/sort/*.cpp)
SET( sort_srcs ${sort_decl} ${sort_impl} )
INSTALL( FILES ${sort_decl} DESTINATION include/yocto/sort )
SOURCE_GROUP( sort FILES ${sort_srcs} )

FILE( GLOB threading_decl yocto/threading/*.hpp)
FILE( GLOB threading_impl yocto/threading/*.cpp)
SET( threading_srcs ${threading_decl} ${threading_impl} )
INSTALL( FILES ${threading_decl} DESTINATION include/yocto/threading)
SOURCE_GROUP(threading FILES ${threading_srcs} )

FILE( GLOB threading_scheme_decl yocto/threading/scheme/*.hpp)
FILE( GLOB threading_scheme_impl yocto/threading/scheme/*.cpp)
SET( threading_scheme_srcs ${threading_scheme_decl} ${threading_scheme_impl} )
INSTALL( FILES ${threading_scheme_decl} DESTINATION include/yocto/threading/scheme)
SOURCE_GROUP(threading/scheme FILES ${threading_scheme_srcs} )


FILE( GLOB core_decl yocto/core/*.hpp)
FILE( GLOB core_impl yocto/core/*.cpp)
SET( core_srcs ${core_decl} ${core_impl})
INSTALL( FILES ${core_decl} DESTINATION include/yocto/core )
SOURCE_GROUP( core FILES ${core_srcs} )

FILE( GLOB mem_decl yocto/memory/*.hpp)
FILE( GLOB mem_impl yocto/memory/*.cpp)
SET( mem_srcs ${mem_decl} ${mem_impl} )
INSTALL( FILES ${mem_decl} DESTINATION include/yocto/memory )
SOURCE_GROUP( memory FILES ${mem_srcs} )

FILE( GLOB string_decl yocto/string/*.hpp)
FILE( GLOB string_impl yocto/string/*.cpp)
SET( string_srcs ${string_decl} ${string_impl} )
INSTALL( FILES ${string_decl} DESTINATION include/yocto/string )
SOURCE_GROUP( string FILES ${string_srcs} )

FILE( GLOB container_decl yocto/container/*.hpp)
FILE( GLOB container_impl yocto/container/*.cpp)
SET( container_srcs ${container_decl} ${container_impl} )
INSTALL( FILES ${container_decl} DESTINATION include/yocto/container )
SOURCE_GROUP( container FILES ${container_srcs} )

FILE( GLOB sequence_decl yocto/sequence/*.hpp)
FILE( GLOB sequence_impl yocto/sequence/*.cpp)
SET( sequence_srcs ${sequence_decl} ${sequence_impl} )
INSTALL( FILES ${sequence_decl} DESTINATION include/yocto/sequence )
SOURCE_GROUP( sequence FILES ${sequence_srcs} )

FILE( GLOB hashing_decl yocto/hashing/*.hpp)
FILE( GLOB hashing_impl yocto/hashing/*.cpp)
SET( hashing_srcs ${hashing_decl} ${hashing_impl} )
INSTALL( FILES ${hashing_decl} DESTINATION include/yocto/hashing )
SOURCE_GROUP( hashing FILES ${hashing_srcs} )

FILE( GLOB ios_decl yocto/ios/*.hpp)
FILE( GLOB ios_impl yocto/ios/*.cpp)
SET( ios_srcs ${ios_decl} ${ios_impl} )
INSTALL( FILES ${ios_decl} DESTINATION include/yocto/ios )
SOURCE_GROUP( ios FILES ${ios_srcs} )

FILE( GLOB randomized_decl yocto/randomized/*.hpp)
FILE( GLOB randomized_impl yocto/randomized/*.cpp)
SET( randomized_srcs ${randomized_decl} ${randomized_impl} )
INSTALL( FILES ${randomized_decl} DESTINATION include/yocto/randomized )
SOURCE_GROUP( randomized FILES ${randomized_srcs} )

FILE( GLOB associative_decl yocto/associative/*.hpp)
FILE( GLOB associative_impl yocto/associative/*.cpp)
SET( associative_srcs ${associative_decl} ${associative_impl} )
INSTALL( FILES ${associative_decl} DESTINATION include/yocto/associative )
SOURCE_GROUP( associative FILES ${associative_srcs} )

FILE( GLOB stock_decl yocto/stock/*.hpp)
FILE( GLOB stock_impl yocto/stock/*.cpp)
SET( stock_srcs ${stock_decl} ${stock_impl} )
INSTALL( FILES ${stock_decl} DESTINATION include/yocto/stock )
SOURCE_GROUP( stock FILES ${stock_srcs} )


FILE( GLOB ordered_decl yocto/ordered/*.hpp)
FILE( GLOB ordered_impl yocto/ordered/*.cpp)
SET( ordered_srcs ${ordered_decl} ${ordered_impl} )
INSTALL( FILES ${ordered_decl} DESTINATION include/yocto/ordered )
SOURCE_GROUP( ordered FILES ${ordered_srcs} )

FILE( GLOB fragment_decl yocto/fragment/*.hpp)
FILE( GLOB fragment_impl yocto/fragment/*.cpp)
SET( fragment_srcs ${fragment_decl} ${fragment_impl} )
INSTALL( FILES ${fragment_decl} DESTINATION include/yocto/fragment )
SOURCE_GROUP( fragment FILES ${fragment_srcs} )

FILE( GLOB math_decl yocto/math/*.h??)
FILE( GLOB math_impl yocto/math/*.cpp)
SET( math_srcs ${math_decl} ${math_impl} )
INSTALL( FILES ${math_decl} DESTINATION include/yocto/math )
SOURCE_GROUP( math FILES ${math_srcs} )

FILE( GLOB rtld_decl yocto/rtld/*.hpp)
FILE( GLOB rtld_impl yocto/rtld/*.cpp)
SET( rtld_srcs ${rtld_decl} ${rtld_impl} )
INSTALL( FILES ${rtld_decl} DESTINATION include/yocto/rtld )
SOURCE_GROUP( rtld FILES ${rtld_srcs} )

FILE( GLOB counting_decl yocto/counting/*.hpp)
FILE( GLOB counting_impl yocto/counting/*.cpp)
SET( counting_srcs ${counting_decl} ${counting_impl} )
INSTALL( FILES ${counting_decl} DESTINATION include/yocto/counting )
SOURCE_GROUP( counting FILES ${counting_srcs} )


FILE( GLOB fs_decl yocto/fs/*.hpp)
FILE( GLOB fs_impl yocto/fs/*.cpp)
SET( fs_srcs ${fs_decl} ${fs_impl} )
INSTALL( FILES ${fs_decl} DESTINATION include/yocto/fs )
SOURCE_GROUP( fs FILES ${fs_srcs} )

FILE( GLOB pack_decl yocto/pack/*.hpp)
FILE( GLOB pack_impl yocto/pack/*.cpp)
SET( pack_srcs ${pack_decl} ${pack_impl} )
INSTALL( FILES ${pack_decl} DESTINATION include/yocto/pack )
SOURCE_GROUP( pack FILES ${pack_srcs} )

FILE( GLOB physics_decl yocto/physics/*.hpp)
FILE( GLOB physics_impl yocto/physics/*.cpp)
SET( physics_srcs ${physics_decl} ${physics_impl} )
INSTALL( FILES ${physics_decl} DESTINATION include/yocto/physics )
SOURCE_GROUP( physics FILES ${physics_srcs} )

#FILE( GLOB parallel_decl yocto/parallel/*.hpp)
#FILE( GLOB parallel_impl yocto/parallel/*.cpp)
#SET( parallel_srcs ${parallel_decl} ${parallel_impl} )
#INSTALL( FILES ${parallel_decl} DESTINATION include/yocto/parallel )
#SOURCE_GROUP( parallel FILES ${parallel_srcs} )

FILE( GLOB ipso_decl yocto/ipso/*.hpp)
FILE( GLOB ipso_impl yocto/ipso/*.cpp)
SET( ipso_srcs ${ipso_decl} ${ipso_impl} )
INSTALL( FILES ${ipso_decl} DESTINATION include/yocto/ipso )
SOURCE_GROUP( ipso FILES ${ipso_srcs} )

FILE( GLOB ipso_mesh_decl yocto/ipso/mesh/*.hpp)
FILE( GLOB ipso_mesh_impl yocto/ipso/mesh/*.cpp)
SET( ipso_mesh_srcs ${ipso_mesh_decl} ${ipso_mesh_impl} )
INSTALL( FILES ${ipso_mesh_decl} DESTINATION include/yocto/ipso/mesh )
SOURCE_GROUP( ipso/mesh FILES ${ipso_mesh_srcs} )

FILE( GLOB ipso_format_decl yocto/ipso/format/*.hpp)
FILE( GLOB ipso_format_impl yocto/ipso/format/*.cpp)
SET( ipso_format_srcs ${ipso_format_decl} ${ipso_format_impl} )
INSTALL( FILES ${ipso_format_decl} DESTINATION include/yocto/ipso/format )
SOURCE_GROUP( ipso/format FILES ${ipso_format_srcs} )

FILE( GLOB mpl_decl yocto/mpl/*.hpp)
FILE( GLOB mpl_impl yocto/mpl/*.cpp)
SET( mpl_srcs ${mpl_decl} ${mpl_impl} )
INSTALL( FILES ${mpl_decl} DESTINATION include/yocto/mpl )
SOURCE_GROUP( mpl FILES ${mpl_srcs} )

FILE( GLOB mpl_natural_decl yocto/mpl/natural/*.hpp)
FILE( GLOB mpl_natural_impl yocto/mpl/natural/*.cpp)
SET( mpl_natural_srcs ${mpl_natural_decl} ${mpl_natural_impl} )
INSTALL( FILES ${mpl_natural_decl} DESTINATION include/yocto/mpl/natural )
SOURCE_GROUP( mpl/natural FILES ${mpl_natural_srcs} )

FILE( GLOB mpl_rsa_decl yocto/mpl/rsa/*.hpp)
FILE( GLOB mpl_rsa_impl yocto/mpl/rsa/*.cpp)
SET( mpl_rsa_srcs ${mpl_rsa_decl} ${mpl_rsa_impl} )
INSTALL( FILES ${mpl_rsa_decl} DESTINATION include/yocto/mpl/rsa )
SOURCE_GROUP( mpl/rsa FILES ${mpl_rsa_srcs} )


SET( decl ${top_decl} ${type_decl} ${ptr_decl} ${code_decl} ${threading_decl} ${threading_scheme_decl} ${core_decl} ${mem_decl} ${string_decl} ${container_decl} ${sequence_decl} ${hashing_decl} ${ios_decl} ${randomized_decl} ${associative_decl} ${stock_decl} ${ordered_decl} ${fragment_decl}  ${math_decl} ${rtld_decl} ${fs_decl} ${pack_decl} ${sort_decl} ${sys_decl} ${counting_decl} ${physics_decl} ${ipso_decl} ${ipso_mesh_decl} ${ipso_format_decl} ${mpl_decl} ${mpl_natural_decl} ${mpl_rsa_decl})
SET( srcs ${top_srcs} ${type_srcs} ${ptr_srcs} ${code_srcs} ${threading_srcs} ${threading_scheme_srcs} ${core_srcs} ${mem_srcs} ${string_srcs} ${container_srcs} ${sequence_srcs} ${hashing_srcs} ${ios_srcs} ${randomized_srcs} ${associative_srcs} ${stock_srcs} ${ordered_srcs} ${fragment_srcs}  ${math_srcs} ${rtld_srcs} ${fs_srcs} ${pack_srcs} ${sort_srcs} ${sys_srcs} ${counting_srcs} ${physics_srcs} ${ipso_srcs} ${ipso_mesh_srcs} ${ipso_format_srcs} ${mpl_srcs} ${mpl_natural_srcs} ${mpl_rsa_impl})

INCLUDE_DIRECTORIES( . )

SET( utest_decl yocto/utest/driver.hpp yocto/utest/run.hpp )
INSTALL( FILES ${utest_decl} DESTINATION include/yocto/utest)
SOURCE_GROUP( utest FILES ${utest_decl})

SET(main_srcs ${srcs} ${utest_decl})
ADD_LIBRARY(yocto STATIC ${main_srcs})

INSTALL( TARGETS yocto ARCHIVE DESTINATION lib )

IF(YOCTO_BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
	ADD_SUBDIRECTORY(tests-mpl)
	ADD_SUBDIRECTORY(tests-ipso)
ENDIF()

ADD_SUBDIRECTORY(dll)
ADD_SUBDIRECTORY(rcpack)
ADD_SUBDIRECTORY(file2data)

YOCTO_BUILD(zlib)
YOCTO_BUILD(bz2)
YOCTO_BUILD(png)
YOCTO_BUILD(jbig)
YOCTO_BUILD(jpeg)
YOCTO_BUILD(lua)
YOCTO_BUILD(lzo)
YOCTO_BUILD(tiff)

ADD_CUSTOM_TARGET(third_party 
DEPENDS 
y-z    test-z
y-bz2  test-bz2
y-png  test-png
y-jbig test-jbig
y-jpeg test-jpeg
y-tiff
y-lua
y-lzo  test-lzo
COMMENT "Building Third Party Libraries")

 

