cmake_minimum_required(VERSION 2.8)

find_package(BISON 3.0)
find_package(FLEX 2.6)
find_package(PythonInterp 3 REQUIRED)

if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_REQUIRED_FLAGS "-std=c++11")
endif()

BISON_TARGET(
                MyParser 
                grammar.y ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.c
                COMPILE_FLAGS "-Wall -d -t -g -r all"
            )
FLEX_TARGET(MyScanner lex.l  ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.c PROPERTIES LANGUAGE CXX )
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c PROPERTIES LANGUAGE CXX )
ADD_LIBRARY( lexgram SHARED
             ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.c ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)

set_property(TARGET lexgram PROPERTY CXX_STANDARD 11)

add_compile_options(-std=c++11 -Wall -Wextra -Werror -DYYDEBUG=0)
# add_executable(
#                   cqasm2openql cqasm2openql.cpp		  
#               )
# add_dependencies(cqasm2openql lexgram)
# target_link_libraries(cqasm2openql lexgram)
# set_property(
#                 TARGET cqasm2openql
#                 PROPERTY CXX_STANDARD 11
#             )
	    

add_executable(
		cqasm2quantumsim cqasm2quantumsim.cpp
		)
		
add_dependencies(cqasm2quantumsim lexgram)
target_link_libraries(cqasm2quantumsim lexgram)
set_property(
                TARGET cqasm2quantumsim
                PROPERTY CXX_STANDARD 11
		)

add_executable(
		cqasm2MYquantumsim cqasm2MYquantumsim.cpp
		)
		
add_dependencies(cqasm2MYquantumsim lexgram)
target_link_libraries(cqasm2MYquantumsim lexgram)
set_property(
                TARGET cqasm2MYquantumsim
                PROPERTY CXX_STANDARD 11
            )	      
			
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs REQUIRED)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

SET_SOURCE_FILES_PROPERTIES(libqasm_swig.i PROPERTIES CPLUSPLUS ON)
SET_PROPERTY(SOURCE libqasm_swig.i PROPERTY SWIG_FLAGS -castmode -modern -keyword)
SWIG_ADD_MODULE(libQasm python libqasm_swig.i)
SWIG_LINK_LIBRARIES(libQasm ${PYTHON_LIBRARIES} lexgram)
