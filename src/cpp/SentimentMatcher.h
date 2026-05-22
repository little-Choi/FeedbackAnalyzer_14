#pragma once

#include "CategoryMatcher.h"
#include "SentimentClassifier.h"

#include <string>

inline std::string classifySentiment(const std::string& text) {
    return SentimentClassifier::classifyUtf8(text);
}

inline bool matchesCategoryMain(const std::string& text, const std::string& category) {
    return CategoryMatcher::matchesMain(text, category);
}
