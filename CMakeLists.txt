cmake_minimum_required (VERSION 2.6)
SET(CMAKE_BUILD_TYPE "Release")
set (PROJECT CrossValidation)

set (HEADERS
#cross.h
    )

set (SOURCES
        main.cpp
#cross.cpp
    )


project (${PROJECT})

# EXTERNAL VARIABLE EXAMPLE.
# There is setting up from cmake<-gui> interface
#
#set(SOME_VARIABLE "" CACHE FILEPATH "Path to something")
#set(SOME_FILE ${SOME_VARIABLE}/foo/bar.h)
#if (NOT EXISTS ${SOME_FILE})
#    message(SEND_ERROR "Can't find foo/bar.h in ${SOME_VARIABLE}")
#endif()

#find_package( Qt5 REQUIRED COMPONENTS Core Gui Widgets Xml Sql)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF(NOT CMAKE_BUILD_TYPE)

    IF((CMAKE_BUILD_TYPE MATCHES "debug") OR (CMAKE_BUILD_TYPE MATCHES "DEBUG"))
        SET(CMAKE_BUILD_TYPE "Debug")
    ENDIF((CMAKE_BUILD_TYPE MATCHES "debug") OR (CMAKE_BUILD_TYPE MATCHES "DEBUG"))

    IF((CMAKE_BUILD_TYPE MATCHES "release") OR (CMAKE_BUILD_TYPE MATCHES "RELEASE"))
        SET(CMAKE_BUILD_TYPE "Release")
    ENDIF((CMAKE_BUILD_TYPE MATCHES "release") OR (CMAKE_BUILD_TYPE MATCHES "RELEASE"))

#enable or disable debugging
    SET(BOOST_LIB_DIR /usr/local/lib)
    IF(CMAKE_BUILD_TYPE MATCHES "Debug")
        ADD_DEFINITIONS(-DDEBUG )
        SET( LINK_LIBRARIES
                ${BOOST_LIB_DIR}/libboost_system.a;${BOOST_LIB_DIR}/libboost_date_time.a;${BOOST_LIB_DIR}/libboost_filesystem.a;${BOOST_LIB_DIR}/libboost_program_options.a;${BOOST_LIB_DIR}/libboost_signals.a;${BOOST_LIB_DIR}/libboost_serialization.a;${BOOST_LIB_DIR}/libboost_thread.a;${BOOST_LIB_DIR}/libboost_unit_test_framework.a
                /usr/local/lib/libshark_debug.a
           )
    ENDIF (CMAKE_BUILD_TYPE MATCHES "Debug")

    IF(CMAKE_BUILD_TYPE MATCHES "Release")
        ADD_DEFINITIONS(-DNDEBUG )
        ADD_DEFINITIONS(-DBOOST_UBLAS_NDEBUG )
        SET( LINK_LIBRARIES
                ${BOOST_LIB_DIR}/libboost_system.a;${BOOST_LIB_DIR}/libboost_date_time.a;${BOOST_LIB_DIR}/libboost_filesystem.a;${BOOST_LIB_DIR}/libboost_program_options.a;${BOOST_LIB_DIR}/libboost_signals.a;${BOOST_LIB_DIR}/libboost_serialization.a;${BOOST_LIB_DIR}/libboost_thread.a;${BOOST_LIB_DIR}/libboost_unit_test_framework.a
                /usr/local/lib/libshark.a
           )
    ENDIF (CMAKE_BUILD_TYPE MATCHES "Release")

#add boost include directory
include_directories ( /usr/local/include/boost )

#handle ATLAS
    OPTION(OPT_ENABLE_ATLAS "Enable Atlas as Linear Algebra library" OFF)
mark_as_advanced(
        OPT_ENABLE_ATLAS
        )

IF ( OPT_ENABLE_ATLAS )
    SET(LINK_LIBRARIES
            ${LINK_LIBRARIES}
            CLAPACK_LIBRARY-NOTFOUND
            CBLAS_LIBRARY-NOTFOUND
            ATLAS_LIBRARY-NOTFOUND
       )
    ADD_DEFINITIONS( -DSHARK_USE_ATLAS )
    include_directories ( /include/ /include/atlas/)
ENDIF ( OPT_ENABLE_ATLAS )

#handle GotoBLAS
    OPTION(OPT_ENABLE_GOTOBLAS "Enable GOTOBLAS as Linear Algebra library" )
mark_as_advanced(
        OPT_ENABLE_GOTOBLAS
        )

IF ( OPT_ENABLE_GOTOBLAS )
    SET(LINK_LIBRARIES 
            ${LINK_LIBRARIES} 


       )
    ADD_DEFINITIONS( -DSHARK_USE_GOTOBLAS )
    include_directories ( /include/ )
ENDIF ( OPT_ENABLE_GOTOBLAS )

#handle OpenMP
    OPTION(OPT_ENABLE_OPENMP "Enable OpenMP" OFF)
mark_as_advanced(
        OPT_ENABLE_OPENMP
        )		
    IF( OPT_ENABLE_OPENMP )
ADD_DEFINITIONS( -DSHARK_USE_OPENMP )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")
ENDIF( OPT_ENABLE_OPENMP )

#include directories
    include_directories (
            /usr/include
            /usr/local/include
            )
####################here Ends shark dependent setup########################

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    add_executable ( ${PROJECT} WIN32 ${HEADERS} ${SOURCES} )
    target_link_libraries ( ${PROJECT} ${LINK_LIBRARIES} )
endif(${CMAKE_SYSTEM_NAME} STREQUAL Windows)

if(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
    add_executable ( ${PROJECT} ${HEADERS} ${SOURCES} )
    target_link_libraries( ${PROJECT} ${LINK_LIBRARIES} )
endif(${CMAKE_SYSTEM_NAME} STREQUAL Linux)

install (TARGETS CrossValidation DESTINATION ${CMAKE_BINARY_DIR}/bin)
