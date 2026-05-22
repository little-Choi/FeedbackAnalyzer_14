#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "AppConfig.h"
#include "Feedback.h"
#include "CategoryMatcher.h"
#include "SentimentClassifier.h"

class Filters {
public:
    [[nodiscard]] std::vector<Feedback> filterBySentimentAndCategory(
        const std::vector<Feedback>& dataList,
        const std::string& sFilter,
        const std::string& kFilter) {
        std::vector<Feedback> afterSentiment = filterBySentiment(dataList, sFilter);
        std::vector<Feedback> result = filterByCategory(afterSentiment, kFilter);
        printFilteredToStdout(result);
        return result;
    }

    // Legacy alias
    [[nodiscard]] std::vector<Feedback> fil(const std::vector<Feedback>& dataList,
                                              const std::string& sFilter,
                                              const std::string& kFilter) {
        return filterBySentimentAndCategory(dataList, sFilter, kFilter);
    }

private:
    [[nodiscard]] static std::vector<Feedback> filterBySentiment(
        const std::vector<Feedback>& dataList,
        const std::string& sFilter) {
        if (sFilter == AppConfig::FILTER_ALL) {
            return dataList;
        }
        std::vector<Feedback> filtered;
        for (const auto& item : dataList) {
            if (SentimentClassifier::classifyUtf8(item.getText()) == sFilter) {
                filtered.push_back(item);
            }
        }
        return filtered;
    }

    [[nodiscard]] static std::vector<Feedback> filterByCategory(
        const std::vector<Feedback>& dataList,
        const std::string& kFilter) {
        if (kFilter == AppConfig::FILTER_ALL) {
            return dataList;
        }
        std::vector<Feedback> filtered;
        for (const auto& item : dataList) {
            if (CategoryMatcher::matchesMain(item.getText(), kFilter)) {
                filtered.push_back(item);
            }
        }
        return filtered;
    }

    static void printFilteredToStdout(const std::vector<Feedback>& filtered) {
        for (const auto& i : filtered) {
            std::cout << i.getText() << std::endl;
        }
    }
};
