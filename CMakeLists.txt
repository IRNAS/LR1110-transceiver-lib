# SPDX-License-Identifier: Apache-2.0

if (NOT DEFINED NO_EXAMPLES)
    cmake_minimum_required(VERSION 3.13.1)

    # This HAS to be above find package command!!!
    # This is only used in case of NRF9160
    # set(spm_DTC_OVERLAY_FILE 
    #     ${CMAKE_CURRENT_SOURCE_DIR}/boards/nrf9160dk_nrf9160spm.overlay)
    # set(spm_CONF_FILE prj.conf 
    #     ${CMAKE_CURRENT_LIST_DIR}/boards/nrf9160dk_nrf9160spm.conf)
    find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})

    project(LR1110_modem_lib)
    zephyr_compile_options(-fdiagnostics-color=always)
    set(CMAKE_BUILD_TYPE Debug)

    # Change line below if you want to change example folder that will be used
    set(EXAMPLE_APPLICATION "version_check")
    # Change line below to the board that you will be using
    add_definitions(-DDEVICE_BOARD="NRF52840")

    set(LR1110_EXAMPLE ${EXAMPLE_APPLICATION})
endif()

zephyr_include_directories(.)
zephyr_include_directories(src)
zephyr_include_directories(src/lr1110_driver)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FILE(GLOB example_dir examples/${LR1110_EXAMPLE}/*.c)
FILE(GLOB app_sources src/*.c)
FILE(GLOB lr1110_driver_sources src/lr1110_driver/*.c)

target_sources(app PRIVATE ${example_dir})
target_sources(app PRIVATE ${app_sources})
target_sources(app PRIVATE ${lr1110_driver_sources})
