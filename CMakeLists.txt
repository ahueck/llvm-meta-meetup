cmake_minimum_required(VERSION 3.20)

project(
    llvm-meta-meetup
    VERSION 0.1.0
    DESCRIPTION "An LLVM-based plugin"
    HOMEPAGE_URL "https://github.com/ahueck/llvm-meta-meetup"
    LANGUAGES CXX C
)

list(
  APPEND
  CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

include(metaToolchain)

meta_target_format(
  format-meta-sources "Formats project source files"
  TARGETS lib/pass/*.cpp
          lib/pass/*.h
)

add_subdirectory(lib)
add_subdirectory(script)

if(PROJECT_IS_TOP_LEVEL)
  enable_testing()
  add_subdirectory(test)
endif()

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/metaConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMinorVersion
)

set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR})
set(LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
set(BINARY_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
set(CMAKE_INSTALL_DIR ${META_INSTALL_CONFIGDIR})

configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/metaConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/metaConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_DIR}
  PATH_VARS INCLUDE_INSTALL_DIR
            LIBRARY_INSTALL_DIR
            BINARY_INSTALL_DIR
            CMAKE_INSTALL_DIR
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/metaConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/metaConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_DIR}
)

if(PROJECT_IS_TOP_LEVEL)
    feature_summary(WHAT ENABLED_FEATURES PACKAGES_FOUND PACKAGES_NOT_FOUND
      DESCRIPTION "llvm-meta-meetup ${PROJECT_VERSION} package and feature info:"
      INCLUDE_QUIET_PACKAGES
      QUIET_ON_EMPTY
    )
endif()
