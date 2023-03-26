cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(temp_sensor)

if (VERBOSE)
  add_compile_definitions(VERBOSE)
endif()

pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.cpp
    TemperatureSensor.cpp
    I2CBusScanner.cpp
    Controller.cpp
    View.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
    pico_stdlib
    pico_multicore
    hardware_i2c
    pico_binary_info
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(temp_sensor)
