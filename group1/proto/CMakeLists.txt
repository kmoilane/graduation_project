cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)

project(Simulation)

include_directories(includes)

add_subdirectory(shm)

add_executable(${PROJECT_NAME} main.cpp shm/shm.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE shm)

add_executable(controller controller/controller.cpp shm/shm.cpp)

set_target_properties(controller PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_include_directories(controller PRIVATE includes)

target_link_libraries(controller PRIVATE shm)

add_executable(conveyor_test tests/conveyor_tests.cpp)

set_target_properties(conveyor_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

target_include_directories(conveyor_test PRIVATE includes)

add_custom_target(test
    COMMAND conveyor_test
    DEPENDS conveyor_test
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(run_both
    COMMAND ./run_simulation.sh
    DEPENDS Simulation controller
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(run
    COMMAND ./Simulation
    DEPENDS Simulation
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
