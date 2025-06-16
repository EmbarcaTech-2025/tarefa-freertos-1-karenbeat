cmake_minimum_required(VERSION 3.13)
project(FreeRTOSpico C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)       # gera compile_commands.json
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")        # executável .elf para o picotool

# --- 1) Raspberry Pi Pico SDK ---
include(pico_sdk_import.cmake)
pico_sdk_init()

# --- 2) Caminho para o FreeRTOS Kernel ---
set(FREERTOS_PATH "${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel-main")

# --- 3) Definição do executável + fontes do FreeRTOS ---
add_executable(${PROJECT_NAME}
    src/main.c

    # FreeRTOS core
    ${FREERTOS_PATH}/croutine.c
    ${FREERTOS_PATH}/event_groups.c
    ${FREERTOS_PATH}/list.c
    ${FREERTOS_PATH}/queue.c
    ${FREERTOS_PATH}/stream_buffer.c
    ${FREERTOS_PATH}/tasks.c
    ${FREERTOS_PATH}/timers.c

    # Porta RP2040
    ${FREERTOS_PATH}/portable/ThirdParty/GCC/RP2040/port.c

    # heap_4
    ${FREERTOS_PATH}/portable/MemMang/heap_4.c
)

# --- 4) Includes de headers ---
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include                                  # FreeRTOSConfig.h
    ${FREERTOS_PATH}/include                                           # FreeRTOS.h, task.h…
    ${FREERTOS_PATH}/portable/ThirdParty/GCC/RP2040/include            # freertos_sdk_config.h
    ${FREERTOS_PATH}/portable/GCC/ARM_CM0plus                           # portmacro.h
)

# --- 5) Link com bibliotecas do Pico ---
target_link_libraries(${PROJECT_NAME}
    pico_stdlib            # SDK do Pico (GPIO, UART, etc.)
    hardware_gpio          # controle de GPIO
    hardware_timer         # timers (se precisar)
    hardware_irq           # interrupções (se precisar)
    hardware_exception     # para expor hardware/exception.h
)

# --- 6) Configurações de stdio e saídas extras ---
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_add_extra_outputs(${PROJECT_NAME})

# --- 7) Warnings extras de compilação ---
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
