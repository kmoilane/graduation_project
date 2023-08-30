cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

project(simulation)

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

add_executable(all_tests tests/tests.cpp)

set_target_properties(all_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

target_include_directories(all_tests PRIVATE includes)

add_custom_target(test
    COMMAND all_tests
    DEPENDS all_tests
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_executable(conveyor_test tests/conveyor_tests.cpp)

set_target_properties(conveyor_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

target_include_directories(conveyor_test PRIVATE includes)

add_custom_target(test_conveyor
    COMMAND conveyor_test
    DEPENDS conveyor_test
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_executable(heater_test tests/heater_tests.cpp)

set_target_properties(heater_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

target_include_directories(heater_test PRIVATE includes)

add_custom_target(test_heater
    COMMAND heater_test
    DEPENDS heater_test
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_executable(cooler_test tests/cooler_tests.cpp)

set_target_properties(cooler_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

target_include_directories(cooler_test PRIVATE includes)

add_custom_target(test_cooler
    COMMAND cooler_test
    DEPENDS cooler_test
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_executable(sim_test tests/simulation_step_test.cpp)

set_target_properties(sim_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

target_include_directories(sim_test PRIVATE includes)

add_custom_target(test_sim
    COMMAND sim_test
    DEPENDS sim_test
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(run_both
    COMMAND ./run_simulation.sh
    DEPENDS ${PROJECT_NAME} controller
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(run
    COMMAND ./${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)