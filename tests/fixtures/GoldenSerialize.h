#pragma once

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Feedback.h"

inline std::string serializeSentimentMap(const std::map<std::string, int>& m) {
    std::ostringstream out;
    out << "# sentiment\n";
    for (const auto& e : m) {
        out << e.first << "=" << e.second << "\n";
    }
    return out.str();
}

inline std::string serializeKeywordMap(const std::map<std::string, int>& m) {
    std::ostringstream out;
    out << "# keyword\n";
    for (const auto& e : m) {
        out << e.first << "=" << e.second << "\n";
    }
    return out.str();
}

inline std::string serializeFeedbackTexts(const std::vector<Feedback>& items) {
    std::ostringstream out;
    out << "# feedbacks\n";
    for (const auto& item : items) {
        out << item.getText() << "\n";
    }
    return out.str();
}

inline std::string serializeTextLines(const std::vector<std::string>& lines) {
    std::ostringstream out;
    out << "# lines\n";
    for (const auto& line : lines) {
        out << line << "\n";
    }
    return out.str();
}
