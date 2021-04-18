cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(firmata_test_project C CXX ASM)
set(CMAKE_C_STANDART 11)
set(CMAKE_CXX_STANDART 17)
pico_sdk_init()

add_executable(firmata_test
    main.cpp
    Firmata.cpp
    Serial.cpp
)

pico_add_extra_outputs(firmata_test)

target_link_libraries(firmata_test pico_stdlib)
