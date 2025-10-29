cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(hgttg_guide C CXX ASM)
set(CMAKE_C_STANDARD 11)
pico_sdk_init()

add_executable(hgttg_guide 
    main_bootloader.c
    enhanced_display.c
    dont_panic_image.c
    dont_panic_data.c 
)

target_link_libraries(hgttg_guide 
    pico_stdlib 
    hardware_spi 
    hardware_i2c 
    hardware_gpio
)

pico_add_extra_outputs(hgttg_guide)