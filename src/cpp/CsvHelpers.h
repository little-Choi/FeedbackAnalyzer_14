#pragma once

#include "AppConfig.h"
#include "Feedback.h"

#include <cctype>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

[[nodiscard]] inline std::string trimCsvField(std::string_view value) {
    const auto start = value.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return {};
    }
    const auto end = value.find_last_not_of(" \t\r\n");
    return std::string(value.substr(start, end - start + 1));
}

inline void stripUtf8Bom(std::string& line) {
    if (line.size() >= 3 &&
        static_cast<unsigned char>(line[0]) == 0xEF &&
        static_cast<unsigned char>(line[1]) == 0xBB &&
        static_cast<unsigned char>(line[2]) == 0xBF) {
        line.erase(0, 3);
    }
}

inline std::string toLowerAscii(std::string value) {
    for (char& c : value) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return value;
}

inline bool fieldEqualsTextColumn(const std::string& field) {
    return toLowerAscii(trimCsvField(field)) == "text";
}

inline bool fieldEqualsIdColumn(const std::string& field) {
    return toLowerAscii(trimCsvField(field)) == "id";
}

inline std::vector<std::string> parseCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(trimCsvField(field));
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(trimCsvField(field));
    return fields;
}

inline int findTextColumnIndex(const std::vector<std::string>& fields) {
    for (size_t i = 0; i < fields.size(); ++i) {
        if (fieldEqualsTextColumn(fields[i])) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

inline bool rowLooksLikeHeader(const std::vector<std::string>& fields) {
    for (const auto& f : fields) {
        if (fieldEqualsTextColumn(f) || fieldEqualsIdColumn(f)) {
            return true;
        }
    }
    return false;
}

inline bool fieldIsNumericId(const std::string& field) {
    const std::string trimmed = trimCsvField(field);
    if (trimmed.empty()) {
        return false;
    }
    for (unsigned char c : trimmed) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

inline int inferTextColumnIndex(const std::vector<std::string>& fields, int defaultIndex) {
    const int explicitText = findTextColumnIndex(fields);
    if (explicitText >= 0) {
        return explicitText;
    }
    if (fields.size() >= 2 && fieldIsNumericId(fields[0])) {
        return 1;
    }
    return defaultIndex;
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
        if (firstLine) {
            stripUtf8Bom(line);
        }
        auto fields = parseCsvLine(line);
        if (fields.empty()) {
            continue;
        }
        if (firstLine) {
            firstLine = false;
            if (rowLooksLikeHeader(fields)) {
                textColumnIndex = inferTextColumnIndex(fields, 0);
                if (textColumnIndex < 0) {
                    textColumnIndex = 0;
                }
                continue;
            }
            textColumnIndex = inferTextColumnIndex(fields, 0);
        }
        if (textColumnIndex >= 0 &&
            static_cast<size_t>(textColumnIndex) < fields.size() &&
            !fields[static_cast<size_t>(textColumnIndex)].empty()) {
            texts.push_back(fields[static_cast<size_t>(textColumnIndex)]);
        }
    }
    return texts;
}

inline void appendFeedbackFromCsvContent(const std::string& fileContent,
                                         std::vector<Feedback>& feedbacks) {
    for (const auto& text : csvUploadTexts(fileContent)) {
        feedbacks.emplace_back(text);
    }
}

inline std::string buildDownloadCsv(const std::vector<Feedback>& filData) {
    std::ostringstream csv;
    csv << AppConfig::UTF8_BOM;
    csv << AppConfig::CSV_TEXT_HEADER << "\n";
    for (const auto& iter : filData) {
        csv << iter.getText() << "\n";
    }
    return csv.str();
}
