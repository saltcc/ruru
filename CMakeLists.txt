project(ruru)
cmake_minimum_required(VERSION 3.7)

option(WITH_TESTS "Build tests" OFF)
set(EXAMPLES_DIR ${CMAKE_CURRENT_BINARY_DIR}/examples)

add_library(ruru
    RuruArena.cpp
    RuruRingBuf.cpp
    RuruCert.cpp
    RuruDtls.cpp
    RuruSctp.cpp
    RuruClient.cpp
    RuruNetwork.cpp
    RuruLoop.cpp
)

set(CXX_FLAGS
    -g
    # -DVALGRIND
    # -DMUDUO_STD_STRING
    #-DCHECK_PTHREAD_RETURN_VALUE
    #-D_FILE_OFFSET_BITS=64
    -Wall
    #-Wextra
    -Werror
    #-Wconversion
    #-Wno-unused-parameter
    #-Wold-style-cast
    #-Woverloaded-virtual
    #-Wpointer-arith
    #-Wshadow
    #-Wwrite-strings
    #-march=native
    # -MMD
    # -std=c++0x
    -std=c++11
    #-rdynamic
)

#set(CMAKE_CXX_COMPILER "g++")
#set(CMAKE_CXX_FLAGS_DEBUG "-O0")
#set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#find_package(Boost REQUIRED)
#find_package(Protobuf)
#find_package(CURL)
#find_package(ZLIB)
#find_path(CARES_INCLUDE_DIR ares.h)
#find_library(CARES_LIBRARY NAMES cares)
#find_path(MHD_INCLUDE_DIR microhttpd.h)
#find_library(MHD_LIBRARY NAMES microhttpd)
#find_library(BOOSTTEST_LIBRARY NAMES boost_unit_test_framework)
#find_library(BOOSTPO_LIBRARY NAMES boost_program_options)
#find_path(TCMALLOC_INCLUDE_DIR gperftools/heap-profiler.h)
#find_library(TCMALLOC_LIBRARY NAMES tcmalloc_and_profiler)
#find_path(HIREDIS_INCLUDE_DIR hiredis/hiredis.h)
#find_library(HIREDIS_LIBRARY NAMES hiredis)
#find_path(GD_INCLUDE_DIR gd.h)
#find_library(GD_LIBRARY NAMES gd)
#find_program(THRIFT_COMPILER thrift)
#find_path(THRIFT_INCLUDE_DIR thrift)
#find_library(THRIFT_LIBRARY NAMES thrift)

#if(CARES_INCLUDE_DIR AND CARES_LIBRARY)
#  message(STATUS "found cares")
#endif()
#if(CURL_FOUND)
#  message(STATUS "found curl")
#endif()
#if(PROTOBUF_FOUND)
#  message(STATUS "found protobuf")
#endif()
#if(TCMALLOC_INCLUDE_DIR AND TCMALLOC_LIBRARY)
#  message(STATUS "found tcmalloc")
#endif()
#if(ZLIB_FOUND)
#  message(STATUS "found zlib")
#endif()
#if(HIREDIS_INCLUDE_DIR AND HIREDIS_LIBRARY)
#  message(STATUS "found hiredis")
#endif()
#if(GD_INCLUDE_DIR AND GD_LIBRARY)
#  message(STATUS "found gd")
#endif()
#if(THRIFT_COMPILER AND THRIFT_INCLUDE_DIR AND THRIFT_LIBRARY)
#  message(STATUS "found thrift")
#endif()



target_include_directories(ruru
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(ruru
    PRIVATE
    -Wall
    -fPIC
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
)

target_link_libraries(ruru
    -lssl
    -lcrypto
    -lusrsctp
    -lpthread
    -lm
)

install(TARGETS ruru EXPORT ruruTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    PUBLIC_HEADER DESTINATION include
)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include)

add_executable(EchoServer examples/EchoServer.cpp)
target_link_libraries(EchoServer ruru)

add_executable(Client examples/EchoClient.cpp)
target_link_libraries(Client ruru)

set_target_properties(ruru EchoServer Client PROPERTIES
    CXX_STANDARD 11
    RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_DIR}
)

file(MAKE_DIRECTORY ${EXAMPLES_DIR})

if (WITH_TESTS)
    
endif()
