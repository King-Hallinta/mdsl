cmake_minimum_required(VERSION 3.10)

project(MDSL
    VERSION 1.0.0
    DESCRIPTION "Meta Domain-Specific Language Library for C++"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE MDSL_HEADERS "include/mdsl/*.h")
file(GLOB_RECURSE MDSL_SOURCES "src/*.cpp")

add_library(MDSL STATIC ${MDSL_SOURCES} ${MDSL_HEADERS})

target_include_directories(MDSL PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(MSVC)
    target_compile_options(MDSL PRIVATE /W4)
else()
    target_compile_options(MDSL PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(MDSL PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/MDSL.h"
)

# Installation
include(GNUInstallDirs)

install(TARGETS MDSL
    EXPORT MDSLTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/mdsl
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT MDSLTargets
    FILE MDSLTargets.cmake
    NAMESPACE MDSL::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MDSL
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MDSLConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MDSL
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/MDSLConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MDSLConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MDSLConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MDSL
)

if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

message(STATUS "MDSL v${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
