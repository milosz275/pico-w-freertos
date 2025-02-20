cmake_minimum_required(VERSION 3.25)
set(PROGRAM_NAME PicoWFreeRTOS)

set(PICO_BOARD "pico_w")
include(cmake/utils.cmake)

file(GLOB PICO_SDK_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/pico-sdk/*")
if(DEFINED ENV{PICO_SDK_PATH})
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/pico-sdk AND PICO_SDK_FILES)
    set(PICO_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/pico-sdk)
else()
    message(FATAL_ERROR "Pico SDK path not found")
endif()

file(GLOB PICO_EXTRAS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/pico-extras/*")
if(DEFINED ENV{PICO_EXTRAS_PATH})
    set(PICO_EXTRAS_PATH $ENV{PICO_EXTRAS_PATH})
elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/pico-extras AND PICO_EXTRAS_FILES)
    set(PICO_EXTRAS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/pico-extras)
else()
    message(FATAL_ERROR "Pico Extras path not found")
endif()

file(GLOB FREERTOS_KERNEL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/freertos-kernel/*")
if(DEFINED ENV{FREERTOS_KERNEL_PATH})
    set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH})
elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/freertos-kernel AND FREERTOS_KERNEL_FILES)
    set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/freertos-kernel)
else()
    message(FATAL_ERROR "FreeRTOS Kernel path not found")
endif()

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)
include(${PICO_EXTRAS_PATH}/pico_sdk_import.cmake)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(${PROGRAM_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(
    -Wall
    -Werror
)

set_source_files_properties(
    ${FREERTOS_KERNEL_PATH}/tasks.c
    ${FREERTOS_KERNEL_PATH}/queue.c
    ${FREERTOS_KERNEL_PATH}/stream_buffer.c
    ${FREERTOS_KERNEL_PATH}/timers.c
    PROPERTIES COMPILE_FLAGS -Wno-error
)

pico_sdk_init()

add_subdirectory(src)
