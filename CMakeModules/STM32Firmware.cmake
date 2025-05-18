#=============================================================================
# SPDX-FileCopyrightText: 2025 Kravchuk Roman
#
# SPDX-License-Identifier: MIT
#=============================================================================

#
# - STM32 firmware build module for CMake
#

function(add_firmware TARGET)
    foreach (arg ${ARGN})
    list(APPEND REAL_SOURCES ${arg})
    endforeach()
    add_executable(${TARGET} ${REAL_SOURCES})
    add_hex_file(${TARGET})
    add_bin_file(${TARGET})
endfunction()

function(add_hex_file TARGET)
    add_custom_command(
        TARGET ${TARGET}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${TARGET}> ${TARGET}${CMAKE_HEX_SUFFIX}
        COMMENT "Creating Intel HEX data file from ${TARGET}${CMAKE_ELF_SUFFIX}"
        )
endfunction()

function(add_bin_file TARGET)
    add_custom_command(
        TARGET ${TARGET}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET}> ${TARGET}${CMAKE_BIN_SUFFIX}
        COMMENT "Creating Intel BIN firmware from ${TARGET}${CMAKE_ELF_SUFFIX}"
        )
endfunction()
