# ---------------------------------------------------------------------------
# Seminar Template
# ---------------------------------------------------------------------------

include(ExternalProject)
find_package(Git REQUIRED)

# Get google benchmark
ExternalProject_Add(
    benchmark_src
    PREFIX "vendor/benchmark"
    INSTALL_DIR "vendor/benchmark"
    GIT_REPOSITORY "https://github.com/google/benchmark.git"
    GIT_TAG e776aa0275e293707b6a0901e0e8d8a8a3679508
    TIMEOUT 10
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/vendor/benchmark
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DGTEST_INCLUDE_DIR=${GTEST_INCLUDE_DIR}
    -DGTEST_LIBRARY=${GTEST_LIBRARY_PATH}
    -DGTEST_MAIN_LIBRARY=${GTEST_MAIN_LIBRARY_PATH}
    -DGMOCK_INCLUDE_DIRS=${GMOCK_INCLUDE_DIR}
    UPDATE_COMMAND ""
)

# Prepare google benchmark
ExternalProject_Get_Property(benchmark_src install_dir)
set(BENCHMARK_INCLUDE_DIR ${install_dir}/include)
set(BENCHMARK_LIBRARY_PATH ${install_dir}/lib/libbenchmark.a)
file(MAKE_DIRECTORY ${BENCHMARK_INCLUDE_DIR})
add_library(benchmark STATIC IMPORTED)
set_property(TARGET benchmark PROPERTY IMPORTED_LOCATION ${BENCHMARK_LIBRARY_PATH})
set_property(TARGET benchmark APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BENCHMARK_INCLUDE_DIR})

# Dependencies
add_dependencies(benchmark_src gtest gmock)
add_dependencies(benchmark benchmark_src)
