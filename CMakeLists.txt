cmake_minimum_required(VERSION 3.16)

# Bare-metal target architecture configuration
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Force the ARM64 cross-compiler for assembly
set(CMAKE_ASM_COMPILER aarch64-linux-gnu-gcc)

project(BareMetalKernel ASM)

# Bare-metal compilation flags
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles")

# Create ELF executable target from boot.S alone
add_executable(kernel8.elf boot.S)

# Tell the linker to run without standard C runtime files
target_link_options(kernel8.elf PRIVATE "-nostdlib" "-nostartfiles")

# Post-build step: Extract raw machine code into kernel8.img
add_custom_command(TARGET kernel8.elf POST_BUILD
    COMMAND aarch64-linux-gnu-objcopy -O binary kernel8.elf ${CMAKE_CURRENT_SOURCE_DIR}/kernel8.img
    COMMENT "Extracting raw binary image: kernel8.img"
)