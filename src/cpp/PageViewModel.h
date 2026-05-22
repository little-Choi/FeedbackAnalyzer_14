#pragma once

#include "Feedback.h"
#include "TrendAnalyzer.h"

#include <map>
#include <string>
#include <vector>

struct PageViewModel {
    std::string success;
    std::string warning;
    std::string error;
    std::map<std::string, int> sentimentResults;
    std::map<std::string, int> keywordResults;
    std::vector<Feedback> feedbacks;
    std::vector<DateSentimentTrend> trendData;
    bool showTrend = false;
    std::map<std::string, std::vector<std::string>> sentimentKeywords;
    std::string adminMessage;
};
