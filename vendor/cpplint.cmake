# ---------------------------------------------------------------------------
# Seminar Template
# ---------------------------------------------------------------------------

include(ExternalProject)
find_package(PythonInterp)

# Get cpplint
ExternalProject_Add(
    cpplint_src
    PREFIX "vendor/cpplint"
    GIT_REPOSITORY "https://github.com/cpplint/cpplint.git"
    GIT_TAG 2a22afe32b7bf4a1ebd7832e203fe8e97c89ee85
    TIMEOUT 10
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
)

# CppLint script path
ExternalProject_Get_Property(cpplint_src source_dir)
set(CPPLINT_SCRIPT_PATH "${source_dir}/cpplint.py")

# Add target to lint the given source files
function(add_cpplint_target TARGET INPUT)
    if (NOT PYTHONINTERP_FOUND)
        message(FATAL_ERROR "CppLint requires a python interpreter")
    endif ()
    if (NOT INPUT)
        add_custom_target(${TARGET})
        return()
    endif ()

    # Remove duplicates & sort
    list(REMOVE_DUPLICATES INPUT)
    list(SORT INPUT)

    # Add target
    add_custom_target(${TARGET}
        COMMAND ${CMAKE_COMMAND} -E chdir
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PYTHON_EXECUTABLE}
        ${CPPLINT_SCRIPT_PATH}
        "--verbose=0"
        "--counting=detailed"
        "--extensions=cc,h"
        "--linelength=200"
        "--filter=-legal/copyright,-runtime/references,-readability/todo,-runtime/printf_format,-whitespace/line_length,-build/c++11"
        ${INPUT}
        DEPENDS cpplint_src
        COMMENT "Running ${TARGET}"
        VERBATIM)
endfunction()
