set(PASS_SOURCES
  MetaPass.cpp
)

meta_llvm_module(meta_AnalysisPass
  "${PASS_SOURCES}"
  INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/lib/type
)
mark_as_advanced(LLVM_META_ANALYSISPASS_LINK_INTO_TOOLS)

set_target_properties(
  meta_AnalysisPass
  PROPERTIES
  OUTPUT_NAME "${PROJECT_NAME}AnalysisPass"
  EXPORT_NAME "AnalysisPass"
)

add_library(meta::AnalysisPass ALIAS meta_AnalysisPass)

target_compile_features(meta_AnalysisPass PUBLIC cxx_std_17)

target_include_directories(meta_AnalysisPass ${warning_guard}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
          ${PROJECT_SOURCE_DIR}/lib/
)

set(CONFIG_NAME metaAnalysisPass)
set(TARGETS_EXPORT_NAME ${CONFIG_NAME}Targets)

install(
  TARGETS meta_AnalysisPass
  EXPORT ${TARGETS_EXPORT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  EXPORT ${TARGETS_EXPORT_NAME}
  NAMESPACE meta::
  DESTINATION ${META_INSTALL_CONFIGDIR}
)

export(
  EXPORT ${TARGETS_EXPORT_NAME}
  FILE ${CMAKE_BINARY_DIR}/${TARGETS_EXPORT_NAME}.cmake
  NAMESPACE meta::
)
