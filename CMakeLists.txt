cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("NesesThread")

# Include sub-projects.
#add_subdirectory ("NesesText")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set (OutputDir "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OutputDir}")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OutputDir}")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OutputDir}")


# Add source to this project's executable.
add_executable (NesesThread "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET NesesThread PROPERTY CXX_STANDARD 17)
endif()

# TODO: Add tests and install targets if needed.