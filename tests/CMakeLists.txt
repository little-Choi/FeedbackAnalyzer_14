include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)

set(DOMAIN_SOURCES
    ${CMAKE_SOURCE_DIR}/src/cpp/Constants.cpp
    ${CMAKE_SOURCE_DIR}/src/cpp/FeedbackRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/cpp/Filters.cpp
    ${CMAKE_SOURCE_DIR}/src/cpp/Logger.cpp
    ${CMAKE_SOURCE_DIR}/src/cpp/Session.cpp
    ${CMAKE_SOURCE_DIR}/src/cpp/TextAnalyzer.cpp
    ${CMAKE_SOURCE_DIR}/src/cpp/UIComponents.cpp
)

file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/expected" FEEDBACK_GOLDEN_EXPECTED_DIR)
string(REPLACE "\\" "/" FEEDBACK_GOLDEN_EXPECTED_DIR "${FEEDBACK_GOLDEN_EXPECTED_DIR}")

set(TEST_SOURCES
    golden/test_golden_outputs.cpp
    unit/test_feedback.cpp
    unit/test_text_analyzer.cpp
    unit/test_filters.cpp
    unit/test_session.cpp
    unit/test_constants.cpp
    unit/test_performance.cpp
    unit/test_logger.cpp
    unit/test_ui_components.cpp
    unit/test_html_escape.cpp
    unit/test_trend_analyzer.cpp
    unit/test_sentiment_keyword_db.cpp
    unit/test_p0_regression.cpp
    integration/test_csv_behavior.cpp
    integration/test_http_routes.cpp
    integration/test_http_exceptions.cpp
    integration/test_trend_http.cpp
)

function(configure_feedback_test_target target_name)
    add_executable(${target_name} ${TEST_SOURCES} ${DOMAIN_SOURCES})
    target_include_directories(${target_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fixtures
        ${CMAKE_CURRENT_SOURCE_DIR}/integration
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_compile_definitions(${target_name} PRIVATE
        GOLDEN_EXPECTED_DIR="${FEEDBACK_GOLDEN_EXPECTED_DIR}"
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

# Golden Master: refresh baselines after intentional behavior change (see docs/golden_master.md)
add_custom_target(update_golden
    COMMAND ${CMAKE_COMMAND} -E env FEEDBACK_ANALYZER_UPDATE_GOLDEN=1
            $<TARGET_FILE:feedback_analyzer_tests> "[golden]"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS feedback_analyzer_tests
    COMMENT "Rewrite tests/expected/* from current outputs"
)

# feedback_analyzer_coverage: run manually after tests (same cases; avoids duplicate CTest entries)
