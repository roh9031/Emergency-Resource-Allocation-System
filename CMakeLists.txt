cmake_minimum_required(VERSION 3.16)
project(ReliefRoute CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

# core library: uncomment each .cpp as it is created. Header-only classes
# (BinarySearch, MergeSort, AllocationStrategy, CostScale) need no entry
add_library(core STATIC
    # domain: problem data with validated invariants
    core/domain/Site.cpp
    core/domain/Depot.cpp
    core/domain/Params.cpp
    # core/domain/RoadGraph.cpp
    # core/domain/Scenario.cpp
    # core/domain/SpatialGrid.cpp
    core/domain/Allocation.cpp

    # algorithms: all graded algorithms
    core/algorithms/FlowNetwork.cpp
    core/algorithms/LinkMaxFlowAllocator.cpp
    core/algorithms/LinkGreedyAllocator.cpp
    # core/algorithms/Dijkstra.cpp
    # core/algorithms/GreedyAllocator.cpp
    # core/algorithms/FlowBasedAllocator.cpp
    # core/algorithms/EdmondsKarpAllocator.cpp
    # core/algorithms/MinCostFlowAllocator.cpp
    # core/algorithms/LexicographicFlowAllocator.cpp
    # core/algorithms/MaxMinFairAllocator.cpp
    # core/algorithms/AllocatorFactory.cpp

    # evaluation: checking, measuring, experiments
    # core/evaluation/AllocationValidator.cpp
    # core/evaluation/MetricsCalculator.cpp
    # core/evaluation/BruteForceSolver.cpp
    # core/evaluation/InstanceGenerator.cpp
    # core/evaluation/ExperimentRunner.cpp
    core/evaluation/Timer.cpp

    # io: files in and out, with validation
    core/io/CsvReader.cpp
    core/io/SampleLoader.cpp
    # core/io/DimacsGraphReader.cpp
    # core/io/ScenarioLoader.cpp
    # core/io/ResultWriter.cpp

    # service: single entry point for the CLI, the server and the tests
    # core/service/AllocationService.cpp
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
    tests/test_binary_search.cpp
    tests/test_allocation_result.cpp
    tests/test_allocation_strategy.cpp
    tests/test_site.cpp
    tests/test_depot.cpp
    tests/test_params.cpp
    tests/test_csv_reader.cpp
    tests/test_sample_loader.cpp
    tests/test_timer.cpp
    tests/test_link_max_flow.cpp
    tests/test_merge_sort.cpp
    tests/test_link_greedy.cpp
)
target_link_libraries(unit-tests PRIVATE core)
# Lets loader tests find data/sample/ no matter where ctest runs from
target_compile_definitions(unit-tests PRIVATE RELIEFROUTE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
add_test(NAME unit-tests COMMAND unit-tests)
# An infinite loop in a flow algorithm fails the test instead of hanging ctest
set_tests_properties(unit-tests PROPERTIES TIMEOUT 30)
