cmake_minimum_required (VERSION 3.28)

project ("ArkhamAsylumDemake" LANGUAGES C CXX)

file (GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
	"Source/*.cpp"
	"Source/*.h"
	"Source/*.hpp"
)

add_executable(ArkhamAsylumDemake ${SRC_FILES})

set_target_properties(ArkhamAsylumDemake PROPERTIES
	CXX_EXTENSIONS NO
	CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE
	CXX_SCAN_FOR_MODULES OFF
)

target_compile_features(ArkhamAsylumDemake PUBLIC cxx_std_23)

target_include_directories(ArkhamAsylumDemake
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
		$<INSTALL_INTERFACE:include>
)

# PS2SDK Configuration - pull from environment variable
if(DEFINED ENV{PS2SDK})
	set(PS2SDK "$ENV{PS2SDK}" CACHE PATH "PS2SDK path")
else()
	set(PS2SDK "/usr/local/ps2dev/ps2sdk" CACHE PATH "PS2SDK path")
endif()

# Link PS2 libraries
target_link_libraries(ArkhamAsylumDemake
	PRIVATE
		stdc++
		dma
		graph
		draw
		kernel
		debug
		packet
		math3d
		pad
		mpeg
		gskit
		dmakit
)

# Add PS2SDK library search paths
target_link_directories(ArkhamAsylumDemake
	PRIVATE
		${PS2SDK}/ee/lib
		${PS2SDK}/ee/common/lib
)

# Compiler and linker flags
target_compile_options(ArkhamAsylumDemake PRIVATE -Wall -Wextra)

message(STATUS "--- System Info ---")
message(STATUS "Target OS:   ${CMAKE_SYSTEM_NAME}")
message(STATUS "Target Arch: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Host OS:     ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "PS2SDK Path: ${PS2SDK}")
message(STATUS "C Compiler:  ${CMAKE_C_COMPILER}")
message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER}")
