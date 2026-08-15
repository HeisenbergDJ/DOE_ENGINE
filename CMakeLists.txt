cmake_minimum_required(VERSION 3.20)

project(DOE_ENGINE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(DOE_SOURCE_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

file(GLOB_RECURSE DOE_SOURCE_TREE_ENTRIES
    CONFIGURE_DEPENDS
    LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*"
)

foreach(DOE_SOURCE_TREE_ENTRY IN LISTS DOE_SOURCE_TREE_ENTRIES)
    if(IS_DIRECTORY "${DOE_SOURCE_TREE_ENTRY}")
        list(APPEND DOE_SOURCE_INCLUDE_DIRS "${DOE_SOURCE_TREE_ENTRY}")
    endif()
endforeach()

list(REMOVE_DUPLICATES DOE_SOURCE_INCLUDE_DIRS)

add_library(doe_config
    src/config/config.cc
    src/utils/json/parser.cc
    src/utils/log/error.cc
)

target_include_directories(doe_config
    PUBLIC
        ${DOE_SOURCE_INCLUDE_DIRS}
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/nlohmann"
)

add_library(doe_runtime
    src/runtime/application/application.cc
    src/runtime/application/process_runner.cc
    src/runtime/application/tagger.cc
    src/runtime/data/design_point.cc
    src/runtime/data/response.cc
    src/runtime/data/scalar_value.cc
    src/runtime/data/variable.cc
    src/runtime/experiment/experiment.cc
    src/runtime/extractor/delimited_text_extractor.cc
    src/runtime/extractor/extractor.cc
    src/runtime/extractor/extractor_util.cc
    src/runtime/extractor/python_script_extractor.cc
    src/runtime/sampling/full_factorial_sampler.cc
    src/runtime/sampling/sampling.cc
    src/utils/tools/delimited_text.cc
    src/utils/tools/path_template.cc
    src/utils/tools/run_context.cc
    src/runtime/workflow/workflow.cc
)

target_include_directories(doe_runtime
    PUBLIC
        ${DOE_SOURCE_INCLUDE_DIRS}
)

target_link_libraries(doe_runtime PUBLIC doe_config)

add_executable(doe_engine
    src/main.cc
)

target_include_directories(doe_engine
    PRIVATE
        ${DOE_SOURCE_INCLUDE_DIRS}
)

target_link_libraries(doe_engine PRIVATE doe_config doe_runtime)

set(DOE_TEST_FIXTURE_OUTPUT_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/tests/fixtures"
)

add_custom_target(copy_test_fixtures ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "${DOE_TEST_FIXTURE_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory
            "${DOE_TEST_FIXTURE_OUTPUT_DIR}/placeholder_text_experiment"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/placeholder_text_experiment"
            "${DOE_TEST_FIXTURE_OUTPUT_DIR}/placeholder_text_experiment"
    COMMAND ${CMAKE_COMMAND} -E remove_directory
            "${DOE_TEST_FIXTURE_OUTPUT_DIR}/delimited_text_experiment"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/delimited_text_experiment"
            "${DOE_TEST_FIXTURE_OUTPUT_DIR}/delimited_text_experiment"
    COMMAND ${CMAKE_COMMAND} -E remove_directory
            "${DOE_TEST_FIXTURE_OUTPUT_DIR}/multiple_extractors_experiment"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/multiple_extractors_experiment"
            "${DOE_TEST_FIXTURE_OUTPUT_DIR}/multiple_extractors_experiment"
    COMMENT "Copying DOE test fixtures"
    VERBATIM
)

add_dependencies(doe_engine copy_test_fixtures)
