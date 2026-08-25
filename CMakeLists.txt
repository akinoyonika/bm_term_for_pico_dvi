cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

set(SUBDIRECTORIES
	PicoDVI/libdvi
)

set(OPTIONS
	-Wall
)

set(DVI_DEFAULT_SERIAL_CONFIG "pico_sock_cfg" CACHE STRING
	"Select a default pin configuration from common_dvi_pin_configs.h"
)

set(DEFINITIONS
	DVI_VERTICAL_REPEAT=1
	DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
	DVI_DEFAULT_PIO_INST=pio1
	PICO_CORE1_STACK_SIZE=0x200
)

set(INCLUDES
	PicoDVI/include
	PicoDVI/libdvi
)

file(GLOB SOURCES
	*.c
)

set(LIBRARIES
	pico_stdlib
	pico_multicore
	libdvi
	tinyusb_host
	tinyusb_board
)

foreach(dir ${SUBDIRECTORIES})
	add_subdirectory(${dir})
endforeach()

add_executable(bm_term ${SOURCES})
target_compile_options(bm_term PRIVATE ${OPTIONS})
target_compile_definitions(bm_term PRIVATE ${DEFINITIONS})
target_include_directories(bm_term PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${INCLUDES})
target_link_libraries(bm_term ${LIBRARIES})

pico_enable_stdio_usb(bm_term 0)
pico_enable_stdio_uart(bm_term 0)

pico_add_extra_outputs(bm_term)
