# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.28)

# Set project name and language
project(asset-packer LANGUAGES C)

# Set C standard to C17, disable compiler-specific extensions and shared libraries
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable Link Time Optimization (if supported)
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT lto_error)
if(lto_supported)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
  message(STATUS "Link Time Optimization (LTO) enabled for Release builds.")
else()
  message(WARNING "Link Time Optimization (LTO) not supported: ${lto_error}")
endif()

# Project options
option(BUILD_TESTS "Build tests" OFF)
option(ENABLE_COMPILE_FLAGS "Enable compile flags" ON)
option(ENABLE_STRIP "Enable symbol stripping for Release builds" ON)

# Enforce out-of-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed. Use a separate build directory.")
endif()

# Set default build type to Release
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Defaulting to 'Release' build type.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Include external CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Include custom modules
include(Flags)

# Optionally enable global ccache
find_program(CCACHE ccache)
if(CCACHE)
  message(STATUS "Ccache enabled for faster builds.")
  set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
else()
  message(WARNING "Ccache not found. Consider installing it to speed up rebuilds.")
endif()

# Get the project version using Git tags if available, else default to "unknown"
set(PROJECT_VERSION "unknown")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git REQUIRED)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --always
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(GIT_TAG)
    set(PROJECT_VERSION ${GIT_TAG})
    message(STATUS "Project version set to ${PROJECT_VERSION} from Git.")
  else()
    message(WARNING "Failed to retrieve Git tag. Using fallback version: ${PROJECT_VERSION}.")
  endif()
else()
  message(WARNING "Git repository not found. Using fallback version: ${PROJECT_VERSION}.")
endif()

# Generate the version header using the inferred Git tag version
configure_file(${CMAKE_SOURCE_DIR}/src/version.h.in ${CMAKE_BINARY_DIR}/generated/version.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}/generated)

# Add the main library target
add_library(${PROJECT_NAME}-lib STATIC
  # find src -name "*.c" ! -name "main.c" | sort
  src/core/args.c
  src/core/prepare.c
)

# Include headers relatively to the src directory
target_include_directories(${PROJECT_NAME}-lib PUBLIC src)

# Apply public compile flags to the library target if enabled
if(ENABLE_COMPILE_FLAGS)
  apply_compile_flags(${PROJECT_NAME}-lib)
endif()

# Add the main executable and link the library
add_executable(${PROJECT_NAME} src/main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}-lib)

# Strip symbols for Release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND ENABLE_STRIP)
  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # macOS
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND strip -x $<TARGET_FILE:${PROJECT_NAME}>
      COMMENT "Stripping local symbols for Release builds on macOS."
    )
    message(STATUS "Stripping local symbols for Release builds on macOS.")
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # GNU/Linux
    target_link_options(${PROJECT_NAME} PRIVATE -s)
    message(STATUS "Stripping symbols for Release builds on GNU/Linux.")
  else()
    # Other platforms, probably Windows (MinGW) or FreeBSD
    message(WARNING "Stripping not configured for this platform.")
  endif()
else()
  if(NOT ENABLE_STRIP)
    message(STATUS "Symbol stripping is disabled by user.")
  else()
    message(STATUS "No stripping applied (non-Release builds).")
  endif()
endif()

# Add install target
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Add tests if enabled
if(BUILD_TESTS)
  # Enable testing with CTest
  enable_testing()

  # Add test executable
  add_executable(tests tests/test_all.c)
  target_link_libraries(tests PRIVATE ${PROJECT_NAME}-lib)

  # Define a function to register tests with CTest
  function(register_test test_name)
    add_test(NAME ${test_name} COMMAND tests ${test_name})
  endfunction()

  # Register tests using the function
  register_test(parse_args)
  register_test(write_header)

  message(STATUS "Tests enabled.")
endif()

# Print the build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}.")
