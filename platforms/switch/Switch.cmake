# CMake Toolchain file for Nintendo Switch using devkitPro / devkitA64
set(CMAKE_SYSTEM_NAME Switch)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

if(NOT DEFINED ENV{DEVKITPRO})
	set(DEVKITPRO "/opt/devkitpro")
else()
	set(DEVKITPRO "$ENV{DEVKITPRO}")
endif()

set(DEVKITA64 "${DEVKITPRO}/devkitA64")
set(LIBNX "${DEVKITPRO}/libnx")
set(PORTLIBS "${DEVKITPRO}/portlibs/switch")

set(CMAKE_C_COMPILER "${DEVKITA64}/bin/aarch64-none-elf-gcc")
set(CMAKE_CXX_COMPILER "${DEVKITA64}/bin/aarch64-none-elf-g++")
set(CMAKE_AR "${DEVKITA64}/bin/aarch64-none-elf-gcc-ar" CACHE STRING "")
set(CMAKE_RANLIB "${DEVKITA64}/bin/aarch64-none-elf-gcc-ranlib" CACHE STRING "")

set(CMAKE_FIND_ROOT_PATH "${PORTLIBS}" "${LIBNX}" "${DEVKITA64}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(SWITCH TRUE)
set(ARCH_ARM64 TRUE)
add_definitions(-D__SWITCH__=1 -D_GNU_SOURCE)

# Switch Tegra X1 architecture flags
set(ARCH_FLAGS "-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE")
set(CMAKE_C_FLAGS_INIT "${ARCH_FLAGS} -I${LIBNX}/include -I${PORTLIBS}/include")
set(CMAKE_CXX_FLAGS_INIT "${ARCH_FLAGS} -I${LIBNX}/include -I${PORTLIBS}/include")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-specs=${LIBNX}/switch.specs -g ${ARCH_FLAGS} -L${LIBNX}/lib -L${PORTLIBS}/lib")
