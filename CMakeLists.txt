cmake_minimum_required(VERSION 3.25)
set(PROGRAM_NAME PicoWFreeRTOS)

set(PICO_BOARD "pico_w")
include(cmake/utils.cmake)

include(pico-sdk/pico_sdk_init.cmake)
include(pico-extras/pico_sdk_import.cmake)

set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(${PROGRAM_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(
    -Wall
    -Werror
)

pico_sdk_init()

add_subdirectory(src)
