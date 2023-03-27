include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(FeatureSummary)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE STRING "" FORCE)
include(FetchContent)

find_package(LLVM CONFIG HINTS "${LLVM_DIR}")
if(NOT LLVM_FOUND)
  message(STATUS "LLVM not found at: ${LLVM_DIR}.")
  find_package(LLVM REQUIRED CONFIG)
endif()

set_package_properties(LLVM PROPERTIES
  URL https://llvm.org/
  TYPE REQUIRED
  PURPOSE
  "LLVM framework installation required to compile (and apply) project llvm-meta-meetup."
)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
  PROJECT_IS_TOP_LEVEL
)

option(META_TEST_CONFIGURE_IDE "Add targets for tests to help the IDE with completion etc." ON)
mark_as_advanced(META_TEST_CONFIGURE_IDE)
option(META_CONFIG_DIR_IS_SHARE "Install to \"share/cmake/\" instead of \"lib/cmake/\"" OFF)
mark_as_advanced(META_CONFIG_DIR_IS_SHARE)

set(warning_guard "")
if(NOT PROJECT_IS_TOP_LEVEL)
  option(
      META_INCLUDES_WITH_SYSTEM
      "Use SYSTEM modifier for meta includes to disable warnings."
      ON
  )
  mark_as_advanced(META_INCLUDES_WITH_SYSTEM)

  if(META_INCLUDES_WITH_SYSTEM)
    set(warning_guard SYSTEM)
  endif()
endif()

include(modules/meta-llvm)
include(modules/meta-format)
include(modules/meta-target-util)

meta_find_llvm_progs(META_CLANG_EXEC "clang-${LLVM_VERSION_MAJOR};clang" DEFAULT_EXE "clang")
meta_find_llvm_progs(META_CLANGCXX_EXEC "clang++-${LLVM_VERSION_MAJOR};clang++" DEFAULT_EXE "clang++")
meta_find_llvm_progs(META_LLC_EXEC "llc-${LLVM_VERSION_MAJOR};llc" DEFAULT_EXE "llc")
meta_find_llvm_progs(META_OPT_EXEC "opt-${LLVM_VERSION_MAJOR};opt" DEFAULT_EXE "opt")

if(PROJECT_IS_TOP_LEVEL)
  if(NOT CMAKE_BUILD_TYPE)
    # set default build type
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
    message(STATUS "Building as debug (default)")
  endif()

  if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    # set default install path
    set(CMAKE_INSTALL_PREFIX
        "${llvm-meta-meetup_SOURCE_DIR}/install/meta"
        CACHE PATH "Default install path" FORCE
    )
    message(STATUS "Installing to (default): ${CMAKE_INSTALL_PREFIX}")
  endif()

    # META_DEBUG_POSTFIX is only used for Config
    if(CMAKE_DEBUG_POSTFIX)
      set(META_DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    else()
      set(META_DEBUG_POSTFIX "-d")
    endif()

  if(NOT CMAKE_DEBUG_POSTFIX AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_DEBUG_POSTFIX ${META_DEBUG_POSTFIX})
  endif()
else()
  set(META_DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
endif()

include(GNUInstallDirs)

set(META_PREFIX ${PROJECT_NAME})
set(TARGETS_EXPORT_NAME ${META_PREFIX}Targets)

if(META_CONFIG_DIR_IS_SHARE)
  set(META_INSTALL_CONFIGDIR ${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME})
else()
  set(META_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
endif()
