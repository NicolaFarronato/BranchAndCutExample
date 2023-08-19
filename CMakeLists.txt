cmake_minimum_required(VERSION 3.26)
project(TestBC)

set(CMAKE_CXX_STANDARD 23)

find_package(fmt REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(Boost REQUIRED program_options)
find_package(CPLEX REQUIRED)
include_directories(${CPLEX_INCLUDE_DIR})
set(SOURCES
        src/main.cpp
        src/LOG/easylogging++.cc
        src/ConfigParams.cpp
        src/Utils/GetOpt.cpp
        src/Instance.cpp
        src/Cvrp_bc.cpp
        src/cvrp_cplex_interface.cpp
)
set(HEADERS
        src/LOG/easylogging++.h
        src/ConfigParams.h
        src/Utils/GetOpt.h
        src/Instance.h
        src/Cvrp_bc.h
        src/cvrp_cplex_interface.h
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})


add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/src/LOG/easylogger_conf.conf
        ${CMAKE_CURRENT_BINARY_DIR}/easylogger_conf.conf)
target_link_libraries(${CMAKE_PROJECT_NAME}  fmt::fmt)
target_link_libraries(${CMAKE_PROJECT_NAME}  rapidjson)
target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${CPLEX_LIBRARIES})
