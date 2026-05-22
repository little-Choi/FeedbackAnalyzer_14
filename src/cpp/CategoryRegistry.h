#pragma once

#include "Constants.h"

#include <string>
#include <vector>

class CategoryRegistry {
public:
    [[nodiscard]] static const std::vector<std::string>* mainKeywords(const std::string& category) {
        if (!Constants::CATEGORY_KEYWORDS.count(category)) {
            return nullptr;
        }
        const auto& catMap = Constants::CATEGORY_KEYWORDS.at(category);
        if (!catMap.count(Constants::CATEGORY_MAIN_KEY)) {
            return nullptr;
        }
        return &catMap.at(Constants::CATEGORY_MAIN_KEY);
    }

    [[nodiscard]] static bool isKnownCategory(const std::string& category) {
        return mainKeywords(category) != nullptr;
    }
};
