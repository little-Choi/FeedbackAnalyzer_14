#pragma once

// Mirrors main.cpp CSV upload/download helpers (keep in sync with src/cpp/main.cpp).
#include "Feedback.h"

#include <sstream>
#include <string>
#include <vector>

inline std::vector<std::string> parseCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(field);
    return fields;
}

inline int findTextColumnIndex(const std::vector<std::string>& fields) {
    for (size_t i = 0; i < fields.size(); ++i) {
        if (fields[i] == "text") {
            return static_cast<int>(i);
        }
    }
    return 0;
}

inline bool rowLooksLikeHeader(const std::vector<std::string>& fields) {
    for (const auto& f : fields) {
        if (f == "text" || f == "id") {
            return true;
        }
    }
    return false;
}

inline std::vector<std::string> csvUploadTexts(const std::string& fileContent) {
    std::vector<std::string> texts;
    std::istringstream stream(fileContent);
    std::string line;
    bool firstLine = true;
    int textColumnIndex = 0;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }
        auto fields = parseCsvLine(line);
        if (fields.empty()) {
            continue;
        }
        if (firstLine) {
            firstLine = false;
            if (rowLooksLikeHeader(fields)) {
                textColumnIndex = findTextColumnIndex(fields);
                continue;
            }
        }
        if (textColumnIndex >= 0 &&
            static_cast<size_t>(textColumnIndex) < fields.size() &&
            !fields[static_cast<size_t>(textColumnIndex)].empty()) {
            texts.push_back(fields[static_cast<size_t>(textColumnIndex)]);
        }
    }
    return texts;
}

inline std::string buildDownloadCsv(const std::vector<Feedback>& filData) {
    std::ostringstream csv;
    csv << "\xEF\xBB\xBF";
    csv << "text\n";
    for (const auto& iter : filData) {
        csv << iter.getText() << "\n";
    }
    return csv.str();
}
