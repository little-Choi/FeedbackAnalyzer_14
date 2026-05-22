#pragma once
#include <string>
#include <vector>
#include <map>

class Constants {
public:
    static constexpr const char* SENTIMENT_POSITIVE = u8"긍정";
    static constexpr const char* SENTIMENT_NEGATIVE = u8"부정";
    static constexpr const char* SENTIMENT_NEUTRAL = u8"중립";
    static constexpr const char* CATEGORY_MAIN_KEY = "main";

    static std::map<std::string, std::vector<std::string>> SENTIMENT_KEYWORDS;
    static std::map<std::string, std::map<std::string, std::vector<std::string>>> CATEGORY_KEYWORDS;

    static void init();
    static std::vector<std::string> categoryNames();
};
