#pragma once

// Replicated from main.cpp (legacy unchanged) for CSV behavior tests.
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

inline std::vector<std::string> csvUploadTexts(const std::string& fileContent) {
    std::vector<std::string> texts;
    std::istringstream stream(fileContent);
    std::string line;
    bool firstLine = true;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (firstLine) {
            firstLine = false;
            continue;
        }
        if (line.empty()) {
            continue;
        }
        auto fields = parseCsvLine(line);
        if (!fields.empty() && !fields[0].empty()) {
            texts.push_back(fields[0]);
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
