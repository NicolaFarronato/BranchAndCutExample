cmake_minimum_required(VERSION 3.26)
project(TestBC)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 23)

find_package(fmt REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(Boost REQUIRED program_options)
find_package(CPLEX REQUIRED)
if(CPLEX_FOUND)
    include_directories(${CPLEX_INCLUDE_DIR})
    link_directories(${CPLEX_LIBRARY})
endif()

if(CPLEX_CONCERT_FOUND)
    include_directories(${CPLEX_CONCERT_INCLUDE_DIR})
    link_directories(${CPLEX_CONCERT_LIBRARY})
endif()

if(CPLEX_ILOCPLEX_FOUND)
    include_directories(${CPLEX_ILOCPLEX_INCLUDE_DIR})
    link_directories(${CPLEX_ILOCPLEX_LIBRARY})
endif()

if(CPLEX_CP_FOUND)
    include_directories(${CPLEX_CP_INCLUDE_DIR})
    link_directories(${CPLEX_CP_LIBRARY})
endif()
set(SOURCES
        src/main.cpp
        src/LOG/easylogging++.cc
        src/ConfigParams.cpp
        src/Utils/GetOpt.cpp
        src/Instance.cpp
        src/Cvrp_bc.cpp
        src/Cvrp_cplex_interface.cpp
)
set(HEADERS
        src/LOG/easylogging++.h
        src/ConfigParams.h
        src/Utils/GetOpt.h
        src/Instance.h
        src/Cvrp_bc.h
        src/Cvrp_cplex_interface.h
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
target_link_libraries(${CMAKE_PROJECT_NAME} cplex-library cplex-concert ilocplex cplex-cp)
