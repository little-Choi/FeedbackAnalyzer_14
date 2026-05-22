#pragma once

#include "CategoryRegistry.h"
#include "Constants.h"

#include <string>
#include <string_view>

class CategoryMatcher {
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

    [[nodiscard]] static bool matchesMain(std::string_view text, const std::string& category) {
        const auto* mainKeywords = CategoryRegistry::mainKeywords(category);
        if (mainKeywords == nullptr) {
            return false;
        }
        return containsAny(text, *mainKeywords);
    }
};
