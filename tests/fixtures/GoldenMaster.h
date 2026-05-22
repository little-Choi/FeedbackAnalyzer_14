#pragma once

#include <catch2/catch_test_macros.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#ifndef GOLDEN_EXPECTED_DIR
#define GOLDEN_EXPECTED_DIR "expected"
#endif

namespace golden {

inline bool updateGoldenEnabled() {
    const char* env = std::getenv("FEEDBACK_ANALYZER_UPDATE_GOLDEN");
    return env != nullptr && env[0] != '\0' && env[0] != '0';
}

inline std::string expectedRoot() {
    return std::string(GOLDEN_EXPECTED_DIR);
}

inline std::string joinPath(const std::string& a, const std::string& b) {
    if (a.empty()) {
        return b;
    }
    if (a.back() == '/' || a.back() == '\\') {
        return a + b;
    }
    return a + "/" + b;
}

inline std::string normalizeNewlines(std::string text) {
    std::string out;
    out.reserve(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\r') {
            if (i + 1 < text.size() && text[i + 1] == '\n') {
                out.push_back('\n');
                ++i;
            } else {
                out.push_back('\n');
            }
        } else {
            out.push_back(text[i]);
        }
    }
    while (!out.empty() && out.back() == '\n') {
        out.pop_back();
    }
    if (!out.empty()) {
        out.push_back('\n');
    }
    return out;
}

inline std::string readFileOrEmpty(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return {};
    }
    std::ostringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

inline void writeFile(const std::string& path, const std::string& content) {
    const auto parent = std::filesystem::path(path).parent_path();
    if (!parent.empty()) {
        std::error_code ec;
        std::filesystem::create_directories(parent, ec);
        REQUIRE(!ec);
    }
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    REQUIRE(out.good());
    out << content;
    out.flush();
    REQUIRE(out.good());
}

inline std::string firstDifferenceLine(const std::string& expected, const std::string& actual) {
    std::istringstream expStream(expected);
    std::istringstream actStream(actual);
    std::string expLine;
    std::string actLine;
    int line = 1;
    while (std::getline(expStream, expLine) || std::getline(actStream, actLine)) {
        if (!expStream && expLine.empty() && actStream) {
            return "actual has extra line " + std::to_string(line) + ": " + actLine;
        }
        if (!actStream && actLine.empty() && expStream) {
            return "missing line " + std::to_string(line) + ": " + expLine;
        }
        if (expLine != actLine) {
            return "line " + std::to_string(line) + " expected=[" + expLine + "] actual=[" + actLine + "]";
        }
        ++line;
    }
    if (expected.size() != actual.size()) {
        return "byte length expected=" + std::to_string(expected.size()) +
               " actual=" + std::to_string(actual.size());
    }
    return "unknown difference";
}

inline void assertMatchesGolden(const std::string& relativePath, const std::string& actualRaw) {
    const std::string expectedPath = joinPath(expectedRoot(), relativePath);
    const std::string actual = normalizeNewlines(actualRaw);

    if (updateGoldenEnabled()) {
        writeFile(expectedPath, actual);
        WARN("Updated golden file: " << expectedPath);
        return;
    }

    const std::string expectedRaw = readFileOrEmpty(expectedPath);
    if (expectedRaw.empty() && !std::ifstream(expectedPath)) {
        FAIL("Missing golden file: " << expectedPath
             << " — run with FEEDBACK_ANALYZER_UPDATE_GOLDEN=1 to create baselines.");
    }

    const std::string expected = normalizeNewlines(expectedRaw);
    if (expected != actual) {
        INFO("Golden path: " << expectedPath);
        INFO(firstDifferenceLine(expected, actual));
        FAIL_CHECK("Golden output mismatch for " << relativePath);
    }
}

}  // namespace golden
