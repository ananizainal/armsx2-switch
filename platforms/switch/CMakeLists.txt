cmake_minimum_required(VERSION 3.16)

message(STATUS "Configuring Nintendo Switch NRO packaging target...")

set(SWITCH_TITLE "ARMSX2")
set(SWITCH_AUTHOR "ARMSX2 Team")
set(SWITCH_VERSION "${ARMSX2_VERSION}")
if(NOT SWITCH_VERSION)
	set(SWITCH_VERSION "1.0.0")
endif()

find_program(ELF2NRO elf2nro PATHS $ENV{DEVKITPRO}/tools/bin NO_DEFAULT_PATH)
find_program(NACPTOOL nacptool PATHS $ENV{DEVKITPRO}/tools/bin NO_DEFAULT_PATH)

if(NOT ELF2NRO)
	find_program(ELF2NRO elf2nro)
endif()

if(NOT NACPTOOL)
	find_program(NACPTOOL nacptool)
endif()

set(SWITCH_ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.png")
set(SWITCH_NACP "${CMAKE_CURRENT_BINARY_DIR}/armsx2.nacp")
set(SWITCH_NRO "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/armsx2.nro")
set(SWITCH_ROMFS_DIR "${CMAKE_SOURCE_DIR}/bin/resources")

if(NACPTOOL)
	add_custom_command(
		OUTPUT ${SWITCH_NACP}
		COMMAND ${NACPTOOL} --create "${SWITCH_TITLE}" "${SWITCH_AUTHOR}" "${SWITCH_VERSION}" ${SWITCH_NACP}
		COMMENT "Creating NACP metadata: ${SWITCH_NACP}"
		VERBATIM
	)
	set(NACP_DEPENDENCY ${SWITCH_NACP})
else()
	set(NACP_DEPENDENCY "")
endif()

if(ELF2NRO)
	add_custom_target(armsx2_nro ALL
		DEPENDS pcsx2-sdl ${NACP_DEPENDENCY}
		COMMAND ${ELF2NRO} "$<TARGET_FILE:pcsx2-sdl>" "${SWITCH_NRO}"
			--icon="${SWITCH_ICON}"
			$<$<BOOL:${NACPTOOL}>:--nacp="${SWITCH_NACP}">
			--romfsdir="${SWITCH_ROMFS_DIR}"
		COMMENT "Packaging executable into Nintendo Switch Homebrew: ${SWITCH_NRO}"
		VERBATIM
	)
else()
	message(WARNING "elf2nro was not found. armsx2.nro target will not be built.")
endif()
