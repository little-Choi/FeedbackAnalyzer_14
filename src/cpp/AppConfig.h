#pragma once

namespace AppConfig {
constexpr int HTTP_PORT = 8080;
constexpr const char* UTF8_BOM = "\xEF\xBB\xBF";
constexpr const char* CSV_TEXT_HEADER = "text";
constexpr const char* CSV_DATE_HEADER = "date";
constexpr const char* FILTER_ALL = u8"전체";
constexpr const char* SENTIMENT_DB_PATH = "data/sentiment_keywords.db";
constexpr const char* TREND_SAMPLE_CSV_PATH = "data/test_feedback_trend.csv";
} // namespace AppConfig
