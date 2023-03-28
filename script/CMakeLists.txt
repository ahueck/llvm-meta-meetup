function(configure_dimeta_script input output)
    cmake_parse_arguments(
      ARG "" "" "TEST_MODE" ${ARGN}
    )
    set(META_SCRIPT_DIR ${CMAKE_BINARY_DIR}/scripts)


    if(${LLVM_VERSION_MAJOR} VERSION_GREATER_EQUAL "13")
        set(META_OPT_ARGS "-enable-new-pm=0")
    endif()

    meta_target_generate_file(${input} ${output})
endfunction()

configure_dimeta_script(apply.tmpl apply${CMAKE_DEBUG_POSTFIX}.sh)

install(
    PROGRAMS
    ${CMAKE_CURRENT_BINARY_DIR}/apply${CMAKE_DEBUG_POSTFIX}.sh
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)