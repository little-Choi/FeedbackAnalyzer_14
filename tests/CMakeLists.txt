include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)

set(DOMAIN_SOURCES
    ${CMAKE_SOURCE_DIR}/src/cpp/Constants.cpp
    ${CMAKE_SOURCE_DIR}/src/cpp/Filters.cpp
    ${CMAKE_SOURCE_DIR}/src/cpp/Logger.cpp
    ${CMAKE_SOURCE_DIR}/src/cpp/Session.cpp
    ${CMAKE_SOURCE_DIR}/src/cpp/TextAnalyzer.cpp
    ${CMAKE_SOURCE_DIR}/src/cpp/UIComponents.cpp
)

set(TEST_SOURCES
    unit/test_feedback.cpp
    unit/test_text_analyzer.cpp
    unit/test_filters.cpp
    unit/test_session.cpp
    unit/test_constants.cpp
    unit/test_performance.cpp
    unit/test_logger.cpp
    unit/test_ui_components.cpp
    unit/test_file_handler.cpp
    unit/test_html_escape.cpp
    integration/test_csv_behavior.cpp
    integration/test_http_routes.cpp
    integration/test_http_exceptions.cpp
)

function(configure_feedback_test_target target_name)
    add_executable(${target_name} ${TEST_SOURCES} ${DOMAIN_SOURCES})
    target_include_directories(${target_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fixtures
        ${CMAKE_CURRENT_SOURCE_DIR}/integration
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_link_libraries(${target_name} PRIVATE Catch2::Catch2WithMain)
    if(WIN32)
        target_compile_definitions(${target_name} PRIVATE
            _WIN32_WINNT=0x0A00
            WINVER=0x0A00
        )
        target_link_libraries(${target_name} PRIVATE ws2_32)
    endif()
endfunction()

configure_feedback_test_target(feedback_analyzer_tests)

option(ENABLE_COVERAGE "Build gcov-instrumented feedback_analyzer_coverage target" OFF)
if(ENABLE_COVERAGE)
    configure_feedback_test_target(feedback_analyzer_coverage)
    target_compile_options(feedback_analyzer_coverage PRIVATE --coverage -O0 -g)
    target_link_options(feedback_analyzer_coverage PRIVATE --coverage)
endif()

include(Catch)
catch_discover_tests(feedback_analyzer_tests
    ADD_TAGS_AS_LABELS
)

# feedback_analyzer_coverage: run manually after tests (same cases; avoids duplicate CTest entries)
