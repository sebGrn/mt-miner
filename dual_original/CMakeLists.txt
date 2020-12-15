
# minimum cmake version
cmake_minimum_required(VERSION 3.10)

# the name of the project
project (dual)

# compiler option
#set (CMAKE_CXX_STANDARD 17)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp") 

# add include directories
#find_package(Boost REQUIRED) 
#find_package(OpenMP)

# set sources and headers
#set(SOURCES ${HEADERS} basic.h box.h stack.h vector.h)
#set(SOURCES ${SOURCES} basic.c box.c stack.c vector.c dual.cpp)

set(SOURCES ${HEADERS} basic.h box.h stack.h vector.h)
set(SOURCES ${SOURCES} stack.c vector.c dual.cpp)

# create static library .a
add_executable(mt_miner ${SOURCES} ${HEADERS})

#target_link_libraries(mt_miner PUBLIC OpenMP::OpenMP_CXX)
#target_link_libraries(mt_miner PRIVATE ${Boost_LIBRARIES} -pthread)

#set_property(TARGET mt_miner PROPERTY POSITION_INDEPENDENT_CODE ON)
#set_property(TARGET mt_miner PROPERTY CXX_STANDARD 17)
  