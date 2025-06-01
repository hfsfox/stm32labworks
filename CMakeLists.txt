#=============================================================================
# SPDX-FileCopyrightText: 2025 Kravchuk Roman
#
# SPDX-License-Identifier: MIT
#=============================================================================

#
# - build file for lab works
#

cmake_minimum_required(VERSION 3.30)

project("STM32Firmware"
        VERSION 0.0.58
        DESCRIPTION "Code for lab works by student ONPU group AT-221"
        HOMEPAGE_URL "http://github.com/hfsfox/stm32labworks/"
        LANGUAGES CXX C ASM
        )

if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(TOOLCHAIN_PREFIX                arm-none-eabi-)
endif()
set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_LINKER                    ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size)
set(CMAKE_BIN_SUFFIX                ".bin")
set(CMAKE_ELF_SUFFIX                ".elf")
set(CMAKE_HEX_SUFFIX                ".hex")
set(CMAKE_EXECUTABLE_SUFFIX_ASM     ${CMAKE_ELF_SUFFIX})
set(CMAKE_EXECUTABLE_SUFFIX_C       ${CMAKE_ELF_SUFFIX})
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ${CMAKE_ELF_SUFFIX})
set(CMAKE_MODULE_PATH "${${PROJECT_NAME}_SOURCE_DIR}/CMakeModules")

include(STM32Firmware)

if(CMAKE_BUILD_STM32F103C8T6)
    set(TARGET_FLAGS "-mcpu=cortex-m3")
    add_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    #USE_HAL_DRIVER
	STM32F103xB
	__TARGET_STM32F103C8T6__
    )

else()
    set(TARGET_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mabi=aapcs -mfpu=fpv4-sp-d16")
    add_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    #USE_HAL_DRIVER
    #STM32F4xx
    STM32F411xE
    __TARGET_STM32F401CEU6__
    )
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-write-strings)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -fdata-sections -ffunction-sections")
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g3")
endif()
if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -g0")
endif()

set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")
set(CMAKE_C_LINK_FLAGS "${TARGET_FLAGS}")
if(CMAKE_BUILD_STM32F103C8T6)
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -T \"${CMAKE_SOURCE_DIR}/Shared/stm32f103c8tx_linker_script.ld\"")
else()
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -T \"${CMAKE_SOURCE_DIR}/Shared/linker_script_f4.ld\"")
endif()
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --specs=nano.specs")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lc -lm -Wl,--end-group")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--print-memory-usage")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--no-warn-rwx-segments")
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lstdc++ -lsupc++ -Wl,--end-group")

message(STATUS "Target platform:        ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler:               ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Target arch:            ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Build type:             ${CMAKE_BUILD_TYPE}")
message(STATUS "Build C flags:          ${CMAKE_C_FLAGS}")
message(STATUS "Build CXX flags:        ${CMAKE_CXX_FLAGS}")
message(STATUS "Link flags:             ${CMAKE_C_LINK_FLAGS}")

if(CMAKE_BUILD_STM32F103C8T6)
include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    include
    CMSIS/CMSIS/Core/Include
    CMSIS/Device/ST/STM32F1/Include
    )
else()
include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    include
    CMSIS/CMSIS/Core/Include
    CMSIS/Device/ST/STM32F4/Include
)
endif()

if(CMAKE_BUILD_STM32F103C8T6)
set(SHARED_SRC
    ../Shared/shared_functions.c
    #../Shared/startup_stm32f103xb.c
    ../Shared/startup_stm32f103xb.s
    ../CMSIS/Device/ST/STM32F1/Source/Templates/system_stm32f1xx.c
    )
else()
set(SHARED_SRC
    ../Shared/shared_functions.c
    ../Shared/startup_stm32f411xx.c
    ../CMSIS/Device/ST/STM32F4/Source/Templates/system_stm32f4xx.c
    )
endif()

add_subdirectory(Lab1)
add_subdirectory(Lab2)
add_subdirectory(Lab3)
add_subdirectory(Lab4)
add_subdirectory(Lab5)
add_subdirectory(Lab6)
add_subdirectory(Lab7)
add_subdirectory(Lab8)
add_subdirectory(CW1)
