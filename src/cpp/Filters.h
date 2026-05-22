#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "Feedback.h"
#include "Constants.h"
#include "SentimentMatcher.h"

class Filters {
public:
    static void initFilterKeywords();

    std::vector<Feedback> fil(const std::vector<Feedback>& dataList,
                              const std::string& sFilter,
                              const std::string& kFilter) {
        std::vector<Feedback> tmpFiltered;

        if (sFilter != u8"전체") {
            for (const auto& item : dataList) {
                std::string txt = item.getText();
                if (classifySentiment(txt) == sFilter) {
                    tmpFiltered.push_back(item);
                }
            }
        } else {
            tmpFiltered = dataList;
        }

        std::vector<Feedback> finalFiltered;
        if (kFilter != u8"전체") {
            for (const auto& item : tmpFiltered) {
                std::string txt = item.getText();
                if (matchesCategoryMain(txt, kFilter)) {
                    finalFiltered.push_back(item);
                }
            }
        } else {
            finalFiltered = tmpFiltered;
        }

        for (const auto& i : finalFiltered) {
            std::cout << i.getText() << std::endl;
        }

        return finalFiltered;
    }
};
