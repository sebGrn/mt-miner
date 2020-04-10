
# minimum cmake version
cmake_minimum_required(VERSION 3.10)

# the name of the project
project (mt_miner)

# compiler option
set (CMAKE_CXX_STANDARD 17)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp") 

# add include directories
#find_package(OpenMP)

# set sources and headers
set(SOURCES ${HEADERS} HypergraphParser.h MT_Miner.h utils.h)
set(SOURCES ${SOURCES} HypergraphParser.cpp MT_Miner.cpp main.cpp )

# create static library .a
add_executable(mt_miner ${SOURCES} ${HEADERS})

#target_link_libraries(mt_miner PUBLIC OpenMP::OpenMP_CXX)

set_property(TARGET mt_miner PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET mt_miner PROPERTY CXX_STANDARD 17)
