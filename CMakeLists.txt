cmake_minimum_required(VERSION 3.16)
project(ReliefRoute CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

# core library add .cpp files here as they are created  
add_library(core STATIC
    core/placeholder.cpp
    core/algorithms/FlowNetwork.cpp
    # core/io.cpp
    # core/search_sort.cpp
    # core/graph.cpp
    # core/greedy.cpp
    # core/flow.cpp
    # core/fair.cpp
    # core/metrics.cpp
    # core/generator.cpp
    # core/experiment.cpp
)
target_include_directories(core PUBLIC core)

# Command-line interface 
add_executable(reliefroute-cli cli/main.cpp)
target_link_libraries(reliefroute-cli PRIVATE core)

# uncomment this code later
# find_package(Threads REQUIRED)
# add_executable(reliefroute-server server/main.cpp server/api.cpp)
# target_include_directories(reliefroute-server PRIVATE third_party)
# target_link_libraries(reliefroute-server PRIVATE core Threads::Threads)

# Tests 
enable_testing()
add_executable(unit-tests
    tests/test_main.cpp
    tests/test_flow_network.cpp
)
target_link_libraries(unit-tests PRIVATE core)
add_test(NAME unit-tests COMMAND unit-tests)
# An infinite loop in a flow algorithm fails the test instead of hanging ctest
set_tests_properties(unit-tests PROPERTIES TIMEOUT 30)
