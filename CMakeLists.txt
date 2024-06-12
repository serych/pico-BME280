cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(pico-BME280 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pico-BME280
	main.c
	)
    
target_sources(pico-BME280 PRIVATE main.c)    

target_link_libraries(pico-BME280 pico_stdlib hardware_spi hardware_clocks)

# enable stdio output to usb, disable to uart
pico_enable_stdio_usb(pico-BME280 1)
pico_enable_stdio_uart(pico-BME280 0)

# Create also uf2 file
pico_add_extra_outputs(pico-BME280)
