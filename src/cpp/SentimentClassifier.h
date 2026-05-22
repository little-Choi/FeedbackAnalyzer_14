#pragma once

#include "Constants.h"
#include "Sentiment.h"

#include <string>
#include <string_view>
#include <vector>

class SentimentClassifier {
public:
    [[nodiscard]] static bool containsAny(std::string_view text,
                                          const std::vector<std::string>& keywords) {
        for (const auto& kw : keywords) {
            if (!kw.empty() && text.find(kw) != std::string_view::npos) {
                return true;
            }
        }
        return false;
    }

    // S4 contract: positive checked before negative (if-else order).
    [[nodiscard]] static Sentiment classify(std::string_view text) {
        if (containsAny(text, Constants::SENTIMENT_KEYWORDS[Constants::SENTIMENT_POSITIVE])) {
            return Sentiment::Positive;
        }
        if (containsAny(text, Constants::SENTIMENT_KEYWORDS[Constants::SENTIMENT_NEGATIVE])) {
            return Sentiment::Negative;
        }
        return Sentiment::Neutral;
    }

    [[nodiscard]] static std::string classifyUtf8(std::string_view text) {
        return sentimentToUtf8(classify(text));
    }
};
