#pragma once

#include "Constants.h"

#include <string>
#include <vector>

inline bool containsAnyKeyword(const std::string& text,
                               const std::vector<std::string>& keywords) {
    for (const auto& kw : keywords) {
        if (!kw.empty() && text.find(kw) != std::string::npos) {
            return true;
        }
    }
    return false;
}

inline std::string classifySentiment(const std::string& text) {
    if (containsAnyKeyword(text, Constants::SENTIMENT_KEYWORDS[u8"긍정"])) {
        return u8"긍정";
    }
    if (containsAnyKeyword(text, Constants::SENTIMENT_KEYWORDS[u8"부정"])) {
        return u8"부정";
    }
    return u8"중립";
}

inline bool matchesCategoryMain(const std::string& text, const std::string& category) {
    if (!Constants::CATEGORY_KEYWORDS.count(category)) {
        return false;
    }
    const auto& catMap = Constants::CATEGORY_KEYWORDS.at(category);
    if (!catMap.count("main")) {
        return false;
    }
    return containsAnyKeyword(text, catMap.at("main"));
}
