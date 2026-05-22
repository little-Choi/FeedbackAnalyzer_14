#pragma once

#include "Constants.h"

#include <string>

enum class Sentiment { Positive, Negative, Neutral };

[[nodiscard]] inline std::string sentimentToUtf8(Sentiment s) {
    switch (s) {
        case Sentiment::Positive:
            return Constants::SENTIMENT_POSITIVE;
        case Sentiment::Negative:
            return Constants::SENTIMENT_NEGATIVE;
        case Sentiment::Neutral:
            return Constants::SENTIMENT_NEUTRAL;
    }
    return Constants::SENTIMENT_NEUTRAL;
}

[[nodiscard]] inline Sentiment sentimentFromUtf8(const std::string& label) {
    if (label == Constants::SENTIMENT_POSITIVE) {
        return Sentiment::Positive;
    }
    if (label == Constants::SENTIMENT_NEGATIVE) {
        return Sentiment::Negative;
    }
    return Sentiment::Neutral;
}
