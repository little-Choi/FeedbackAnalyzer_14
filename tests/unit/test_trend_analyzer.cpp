#include <catch2/catch_test_macros.hpp>

#include "CsvHelpers.h"
#include "DomainFixture.h"
#include "TrendAnalyzer.h"

TEST_CASE("TR-01 csvUploadFeedbackRows parses date,text columns", "[trend][csv]") {
    const std::string csv = std::string(u8"date,text\n2026-05-01,") + u8"배송이 빠르고 좋습니다\n";
    auto rows = csvUploadFeedbackRows(csv);
    REQUIRE(rows.size() == 1);
    REQUIRE(rows[0].getDate() == "2026-05-01");
    REQUIRE(rows[0].getText() == u8"배송이 빠르고 좋습니다");
}

TEST_CASE_METHOD(DomainFixture, "TR-02 TrendAnalyzer aggregates sentiment by date", "[trend]") {
    std::vector<Feedback> rows;
    rows.emplace_back(u8"배송이 빠르고 좋습니다", "2026-05-01");
    rows.emplace_back(u8"배송이 늦었어요", "2026-05-02");
    rows.emplace_back(u8"괜찮아요", "2026-05-02");

    const auto trends = TrendAnalyzer::analyzeByDate(rows);
    REQUIRE(trends.size() == 2);
    REQUIRE(TrendAnalyzer::hasTrendData(rows));

    int day1Positive = 0;
    int day2Negative = 0;
    for (const auto& row : trends) {
        if (row.date == "2026-05-01") {
            day1Positive = row.sentimentCounts.at(u8"긍정");
        }
        if (row.date == "2026-05-02") {
            day2Negative = row.sentimentCounts.at(u8"부정");
        }
    }
    REQUIRE(day1Positive == 1);
    REQUIRE(day2Negative == 1);
}

TEST_CASE("TR-03 test_feedback_trend.csv fixture loads eight rows", "[trend]") {
    const std::string csv = R"(date,text
2026-05-01,)" + std::string(u8"배송이 빠르고 좋습니다") + R"(
2026-05-01,)" + std::string(u8"서비스가 최고입니다") + R"(
2026-05-02,)" + std::string(u8"배송이 늦었어요 화가 납니다") + R"(
2026-05-02,)" + std::string(u8"품질이 별로예요") + R"(
2026-05-03,)" + std::string(u8"괜찮아요") + R"(
2026-05-03,)" + std::string(u8"가격이 저렴해서 좋아요") + R"(
2026-05-04,)" + std::string(u8"불만이 많습니다 실망했어요") + R"(
2026-05-05,)" + std::string(u8"배송 빠르고 만족합니다") + "\n";
    auto rows = csvUploadFeedbackRows(csv);
    REQUIRE(rows.size() == 8);
    REQUIRE(rows[0].hasDate());
    REQUIRE(TrendAnalyzer::hasTrendData(rows));
}

TEST_CASE_METHOD(DomainFixture, "TR-04 renderTrendChartHtml emits chart markup", "[trend]") {
    std::vector<Feedback> rows;
    rows.emplace_back(u8"좋아요", "2026-05-01");
    const auto trends = TrendAnalyzer::analyzeByDate(rows);
    const std::string html = TrendAnalyzer::renderTrendChartHtml(trends);
    REQUIRE(html.find("trend-chart") != std::string::npos);
    REQUIRE(html.find("trend-pos") != std::string::npos);
}
