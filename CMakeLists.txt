cmake_minimum_required(VERSION 3.13)
# Компиляторы
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# Пути к корневой папке пользователя
set(HOME_PATH $ENV{HOME})
if(CMAKE_HOST_WIN32)
    # Преобразование пути к домашней папке из Windows-стиля в Unix
    set(HOST_EXECUTABLE_SUFFIX .exe)
    file(TO_CMAKE_PATH $ENV{HOMEDRIVE}$ENV{HOMEPATH} HOME_PATH)
endif(CMAKE_HOST_WIN32)

# Пути к компилятору, библиотекам CMSIS, скриптами линкера
set (ARM_NONE_EABI_BIN_PATH ${HOME_PATH}/.platformio/packages/toolchain-gccarmnoneeabi/bin)
set (STM32F1_CMSIS_SOURCE_PATH ${HOME_PATH}/.platformio/packages/framework-cmsis-stm32f3/Source/Templates)
set (STM_LINKER_SCRIPTS_PATH ${HOME_PATH}/.platformio/packages/tool-ldscripts-ststm32)

set(CMAKE_C_COMPILER ${ARM_NONE_EABI_BIN_PATH}/arm-none-eabi-gcc${HOST_EXECUTABLE_SUFFIX})
set(CMAKE_CXX_COMPILER ${ARM_NONE_EABI_BIN_PATH}/arm-none-eabi-g++${HOST_EXECUTABLE_SUFFIX})
set(CMAKE_ASM_COMPILER ${ARM_NONE_EABI_BIN_PATH}/arm-none-eabi-gcc${HOST_EXECUTABLE_SUFFIX})
set(CMAKE_AR ${ARM_NONE_EABI_BIN_PATH}/arm-none-eabi-ar${HOST_EXECUTABLE_SUFFIX})
set(CMAKE_OBJCOPY ${ARM_NONE_EABI_BIN_PATH}/arm-none-eabi-objcopy${HOST_EXECUTABLE_SUFFIX})
set(CMAKE_OBJDUMP ${ARM_NONE_EABI_BIN_PATH}/arm-none-eabi-objdump${HOST_EXECUTABLE_SUFFIX})
set(SIZE ${ARM_NONE_EABI_BIN_PATH}/arm-none-eabi-size${HOST_EXECUTABLE_SUFFIX})

# TODO 0: Определить название проекта и поддерживаемые языки
project("Svet" C CXX)

# TODO 1: Определить ядро и название микроконтроллера
set(MCPU cortex-m4)
add_definitions(-DSTM32F303xC)
add_definitions(-DSTM32F3)

# TODO 2: Указать правильные пути к библиотекам, файлам system и startup
include_directories(
                    ${CMAKE_SOURCE_DIR}/include
                    ${CMAKE_SOURCE_DIR}/src
                    ${HOME_PATH}/.platformio/packages/framework-cmsis/CMSIS/Core/Include
                    ${HOME_PATH}/.platformio/packages/framework-cmsis-stm32f3/Include
                    ${HOME_PATH}/.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/include/c++/7.2.1
                    ${HOME_PATH}/.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/include/c++/7.2.1/arm-none-eabi
                    ${HOME_PATH}/.platformio/packages/toolchain-gccarmnoneeabi/lib/gcc/arm-none-eabi/7.2.1/include
                    ${HOME_PATH}/.platformio/packages/toolchain-gccarmnoneeabi/lib/gcc/arm-none-eabi/7.2.1/include-fixed
                    ${HOME_PATH}/.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/include
                    )

file(GLOB_RECURSE SRC_LIST
    ${CMAKE_SOURCE_DIR}/src/*.*
    ${CMAKE_SOURCE_DIR}/lib/*.*
    ${CMAKE_SOURCE_DIR}/test/*.*
    ${STM32F1_CMSIS_SOURCE_PATH}/gcc/startup_stm32f303xc.s
    ${STM32F1_CMSIS_SOURCE_PATH}/system_stm32f3xx.c
    )

# TODO 3: Указать параметры компиляции
add_compile_options(-mcpu=${MCPU} -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)
add_compile_options(-Wall)
#set(CMAKE_CXX_FLAGS -fno-rtti -fno-exceptions)
message(STATUS "\n Build: ${CMAKE_BUILD_TYPE}")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Os)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -ggdb3)
endif ()

# TODO 4: Указать параметры компоновки, правильный скрипт компоновщика
set(LINKER_SCRIPT ${STM_LINKER_SCRIPTS_PATH}/stm32f1/STM32F103CBTX_FLASH.ld)
add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=${MCPU} -mthumb -mthumb-interwork)
add_link_options(-T ${LINKER_SCRIPT})

# Сборка
add_executable(${PROJECT_NAME}.elf ${SRC_LIST} ${LINKER_SCRIPT})
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
