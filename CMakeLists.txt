# only for cmake --version >= 3.12.4
cmake_minimum_required(VERSION 3.12.4)


set(PEDANTIC_WARNINGS false CACHE BOOL "Enable pedantic compiler warnings")

# project name
project(sip-client C CXX)

find_package( Threads REQUIRED )
list(APPEND CMAKE_MODULE_PATH /usr/share/SFML/cmake/Modules/)
find_package(SFML COMPONENTS audio system REQUIRED)


# I../includes
include_directories(lib/sip_client/include lib/argparse src)

# puts all .cpp files inside src to the SOURCES variable
set(SOURCES src/main.cpp)

add_compile_definitions(COMPILE_FOR_NATIVE)

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address  -fsanitize=leak")
#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address  -fsanitize=leak")
#set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address  -fsanitize=leak")
#set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fsanitize=address  -fsanitize=leak")



# compiles the files defined by SOURCES to generate the executable
add_executable(sip-client ${SOURCES})

target_link_libraries(sip-client mbedcrypto ${CMAKE_THREAD_LIBS_INIT} sfml-audio sfml-system)

#if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND ${PEDANTIC_WARNINGS})
#  target_compile_options(sip-client PRIVATE -Wall -Wextra -Weverything -Wno-c++98-compat -Wno-format-nonliteral -Wno-gnu-zero-variadic-macro-arguments -Wno-c++98-compat-pedantic -Wno-deprecated-declarations)
#elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND ${PEDANTIC_WARNINGS})
#  target_compile_options(sip-client PRIVATE -Wall -Wextra -Wno-c++98-compat -Wno-format-nonliteral -Wno-gnu-zero-variadic-macro-arguments -Wno-c++98-compat-pedantic -Wno-deprecated-declarations -pedantic)
#else()
#  target_compile_options(sip-client PRIVATE -Wall -Wextra -Werror -Wno-deprecated-declarations)
#endif()

# set the C++ standard
set(TARGET sip-client PROPERTY CMAKE_CXX_STANDARD_REQUIRED 17)
set_property(TARGET sip-client PROPERTY CXX_STANDARD 17)

