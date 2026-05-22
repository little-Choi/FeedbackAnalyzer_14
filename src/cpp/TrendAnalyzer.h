#pragma once

#include "Constants.h"
#include "Feedback.h"
#include "SentimentClassifier.h"

#include <map>
#include <sstream>
#include <string>
#include <vector>

struct DateSentimentTrend {
    std::string date;
    std::map<std::string, int> sentimentCounts;
};

class TrendAnalyzer {
public:
    [[nodiscard]] static std::vector<DateSentimentTrend> analyzeByDate(
        const std::vector<Feedback>& feedbacks) {
        std::map<std::string, std::map<std::string, int>> byDate;
        for (const auto& fb : feedbacks) {
            if (!fb.hasDate()) {
                continue;
            }
            const std::string sentiment =
                SentimentClassifier::classifyUtf8(fb.getText());
            byDate[fb.getDate()][sentiment]++;
        }
        std::vector<DateSentimentTrend> result;
        for (const auto& entry : byDate) {
            DateSentimentTrend row;
            row.date = entry.first;
            row.sentimentCounts = entry.second;
            result.push_back(std::move(row));
        }
        return result;
    }

    [[nodiscard]] static bool hasTrendData(const std::vector<Feedback>& feedbacks) {
        for (const auto& fb : feedbacks) {
            if (fb.hasDate()) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] static std::string renderTrendChartHtml(
        const std::vector<DateSentimentTrend>& trends) {
        if (trends.empty()) {
            return {};
        }
        int maxTotal = 0;
        for (const auto& row : trends) {
            int total = 0;
            for (const auto& c : row.sentimentCounts) {
                total += c.second;
            }
            if (total > maxTotal) {
                maxTotal = total;
            }
        }
        if (maxTotal <= 0) {
            maxTotal = 1;
        }

        std::ostringstream html;
        html << R"(<div class="trend-chart">)";
        for (const auto& row : trends) {
            int total = 0;
            for (const auto& c : row.sentimentCounts) {
                total += c.second;
            }
            const int barHeight = (total * 100) / maxTotal;
            html << R"(<div class="trend-day"><div class="trend-date">)"
                 << row.date << R"(</div><div class="trend-bar-wrap">)";
            auto appendSegment = [&](const char* label, const char* cssClass) {
                const auto it = row.sentimentCounts.find(label);
                const int count = (it != row.sentimentCounts.end()) ? it->second : 0;
                if (count > 0) {
                    const int segHeight = (count * barHeight) / total;
                    html << R"(<div class="trend-seg )" << cssClass << R"(" style="height:)"
                         << segHeight << R"(px" title=")" << label << ": " << count
                         << R"("></div>)";
                }
            };
            appendSegment(Constants::SENTIMENT_POSITIVE, "trend-pos");
            appendSegment(Constants::SENTIMENT_NEUTRAL, "trend-neu");
            appendSegment(Constants::SENTIMENT_NEGATIVE, "trend-neg");
            html << R"(</div><div class="trend-total">)" << total << "</div></div>";
        }
        html << R"(</div>)";
        return html.str();
    }
};
