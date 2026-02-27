cmake_minimum_required(VERSION 3.12)
project(knight_streamer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BUILD_SHARED_LIBS OFF)
set(SUBMODULE_PATH ${CMAKE_SOURCE_DIR}/thirdparty)


add_executable(${PROJECT_NAME}
    src/main.cpp
    src/utils/print_helpers.cpp
    src/interaction/arguments.cpp
    src/interaction/port_selection.cpp
    src/neuropawn/knight.cpp
    src/neuropawn/knight_imu.cpp
)
target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
target_precompile_headers(${PROJECT_NAME} PRIVATE include/pch.hpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)


add_subdirectory(${SUBMODULE_PATH}/liblsl)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC lsl)

add_subdirectory(${SUBMODULE_PATH}/CSerialPort)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC libcserialport)
target_include_directories(${PROJECT_NAME} PUBLIC ${SUBMODULE_PATH}/CSerialPort/include)


function(INCLUDE_SUBMODULE MODULE_NAME)
    add_library(${MODULE_NAME} INTERFACE)
    target_include_directories(
        ${MODULE_NAME} INTERFACE "${SUBMODULE_PATH}/${MODULE_NAME}/include"
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE ${MODULE_NAME})
endfunction()

include_submodule(argparse)