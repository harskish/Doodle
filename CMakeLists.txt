cmake_minimum_required(VERSION 3.3)
project(Doodle)

find_package(SDL2 REQUIRED)

# Using LLVM 5 (clang)
set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp=libomp -Wall -Werror")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

include_directories(
    ${SDL2_INCLUDE_DIRS}
)

# LLVM libomp
link_directories(/usr/local/opt/llvm/lib)

set(SOURCE_FILES
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
