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

# CONTROLLER
add_executable(controller controller/controller.cpp shm/shm.cpp)

set_target_properties(controller PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_include_directories(controller PRIVATE includes)

target_link_libraries(controller PRIVATE shm)

# UI
add_executable(ui ui/ui.cpp shm/shm.cpp)

set_target_properties(ui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_include_directories(ui PRIVATE includes)

target_link_libraries(ui PRIVATE shm)

# TESTS
add_executable(all_tests tests/tests.cpp)

set_target_properties(all_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

target_include_directories(all_tests PRIVATE includes)

# CUSTOM COMMANDS (usage example: "make run" or "make run_sim" etc.)

add_custom_target(run
    COMMAND ./run_simulation.sh
    DEPENDS ${PROJECT_NAME} controller ui
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(test
    COMMAND all_tests
    DEPENDS all_tests
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(run_ui
    COMMAND ui
    DEPENDS ui
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(run_controller
    COMMAND controller
    DEPENDS controller
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)


add_custom_target(run_sim
    COMMAND ./${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)