# ---------------------------------------------------------------------------
# Seminar Template
# ---------------------------------------------------------------------------

function(add_coverage_target target_name)
    set(${target_name}_COVERAGE_OUTPUT ${target_name})
    set(${target_name}_COVERAGE_IGNORE
        ${${target_name}_COVERAGE_IGNORE}
        ${${target_name}_COVERAGE_OUTPUT}.base.info
        '${CMAKE_SOURCE_DIR}/**/test/*'
        '${CMAKE_SOURCE_DIR}/cmake-build-*/*'
        '${CMAKE_SOURCE_DIR}/build/*'
        '${CMAKE_SOURCE_DIR}/release/*'
        '${CMAKE_SOURCE_DIR}/debug/*'
        '${CMAKE_SOURCE_DIR}/coverage/*'
        '/usr/*')

    if ((CMAKE_BUILD_TYPE STREQUAL "Debug") AND (NOT NO_COVERAGE) AND GCOV_PATH AND LCOV_PATH AND GENHTML_PATH)
        set(test_drivers ${ARGN})

        foreach (test_driver ${test_drivers})
            list(APPEND test_driver_commands COMMAND ${test_driver})
        endforeach (test_driver ${test_drivers})

        add_custom_target(${target_name}
            # cleanup lcov
            COMMAND ${LCOV_PATH} --directory . --zerocounters

            # create baseline coverage file
            COMMAND ${LCOV_PATH} -i --directory . --capture --output-file ${${target_name}_COVERAGE_OUTPUT}.base.info

            # run tests
            ${test_driver_commands}

            # capture counters and generate report
            COMMAND ${LCOV_PATH} --directory . --capture --output-file ${${target_name}_COVERAGE_OUTPUT}.captured.info
            COMMAND ${LCOV_PATH} -a ${${target_name}_COVERAGE_OUTPUT}.base.info -a ${${target_name}_COVERAGE_OUTPUT}.captured.info -o ${${target_name}_COVERAGE_OUTPUT}.info
            COMMAND ${LCOV_PATH} --remove ${${target_name}_COVERAGE_OUTPUT}.info ${${target_name}_COVERAGE_IGNORE} --output-file ${${target_name}_COVERAGE_OUTPUT}.info.cleaned
            COMMAND ${GENHTML_PATH} -o ${${target_name}_COVERAGE_OUTPUT} ${${target_name}_COVERAGE_OUTPUT}.info.cleaned

            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            )

        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ;
            COMMENT "Open ./${${target_name}_COVERAGE_OUTPUT}/index.html in your browser to view the coverage report."
            )
    elseif ((CMAKE_BUILD_TYPE STREQUAL "Debug") AND (NOT NO_COVERAGE))
        if (NOT GCOV_PATH)
            message(WARNING "gcov not found - not generating coverage target")
        elseif (NOT LCOV_PATH)
            message(WARNING "lcov not found - not generating coverage target")
        elseif (NOT GENHTML_PATH)
            message(WARNING "genhtml not found - not generating coverage target")
        endif ()
    endif ()
endfunction(add_coverage_target)