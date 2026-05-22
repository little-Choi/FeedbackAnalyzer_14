#pragma once
#include <string>
#include <vector>
#include <map>
#include "Feedback.h"
#include "Constants.h"
#include "CategoryMatcher.h"
#include "SentimentClassifier.h"

class TextAnalyzer {
public:
    [[nodiscard]] std::map<std::string, int> analyzeSentimentDistribution(
        const std::vector<Feedback>& feedbacks) {
        std::map<std::string, int> res;
        res[Constants::SENTIMENT_POSITIVE] = 0;
        res[Constants::SENTIMENT_NEUTRAL] = 0;
        res[Constants::SENTIMENT_NEGATIVE] = 0;

        for (const auto& f : feedbacks) {
            res[SentimentClassifier::classifyUtf8(f.getText())]++;
        }
        return res;
    }

    [[nodiscard]] std::map<std::string, int> analyzeKeywordDistribution(
        const std::vector<Feedback>& feedbacks) {
        std::map<std::string, int> res2;
        for (const auto& entry : Constants::CATEGORY_KEYWORDS) {
            res2[entry.first] = 0;
        }

        for (const auto& f : feedbacks) {
            const std::string& txt = f.getText();
            for (const auto& entry : Constants::CATEGORY_KEYWORDS) {
                const std::string& cat = entry.first;
                if (entry.second.count(Constants::CATEGORY_MAIN_KEY)) {
                    const auto& kws = entry.second.at(Constants::CATEGORY_MAIN_KEY);
                    if (CategoryMatcher::containsAny(txt, kws)) {
                        res2[cat]++;
                    }
                }
            }
        }
        return res2;
    }

    // Legacy aliases (Phase 3-3); remove when all call sites migrated.
    [[nodiscard]] std::map<std::string, int> sent(const std::vector<Feedback>& feedbacks) {
        return analyzeSentimentDistribution(feedbacks);
    }

    [[nodiscard]] std::map<std::string, int> kw(const std::vector<Feedback>& feedbacks) {
        return analyzeKeywordDistribution(feedbacks);
    }
};
