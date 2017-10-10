cmake_minimum_required(VERSION 2.8)
project(Doodle)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Doodle_SOURCE_DIR}/cmake")
#find_package(SDL2 REQUIRED)

# Temporary fix for DSIC remote desktop
include_directories(${CMAKE_SOURCE_DIR}/../libs/include/SDL2)
link_directories(${CMAKE_SOURCE_DIR}/../libs/lib)
set(SDL2_LIBRARIES SDL2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-unknown-pragmas")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

include_directories(
    ${SDL2_INCLUDE_DIRS}
)

# LLVM libomp
link_directories(/usr/local/opt/llvm/lib)

set(SOURCE_FILES
	src/AnnealingOptimizer.cpp
	src/AnnealingOptimizer.h
	src/Solution.cpp
	src/Solution.h
	src/GeneticOptimizer.cpp
	src/GeneticOptimizer.h
	src/Optimizer.cpp
	src/Optimizer.h
	src/Phenotype.cpp
	src/Phenotype.h
	src/Utils.h
	src/doodle.cpp
	src/targetver.h
)

add_executable(Doodle ${SOURCE_FILES})

target_link_libraries(Doodle
    ${SDL2_LIBRARIES}
)
