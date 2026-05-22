#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

#include "DomainFixture.h"
#include "SentimentClassifier.h"
#include "SentimentKeywordDb.h"

namespace fs = std::filesystem;

TEST_CASE_METHOD(DomainFixture, "DB-01 SentimentKeywordDb save and load roundtrip", "[sentiment-db]") {
    const auto tempPath =
        (fs::temp_directory_path() / "feedback_analyzer_test_sentiment.db").string();
    fs::remove(tempPath);

    Constants::SENTIMENT_KEYWORDS[u8"긍정"] = {u8"테스트긍정키워드"};
    SentimentKeywordDb writer(tempPath);
    REQUIRE(writer.saveFromConstants());

    Constants::init();
    SentimentKeywordDb reader(tempPath);
    REQUIRE(reader.loadIntoConstants());
    REQUIRE(SentimentClassifier::classifyUtf8(u8"테스트긍정키워드문장") == u8"긍정");

    fs::remove(tempPath);
}

TEST_CASE_METHOD(DomainFixture, "DB-02 addKeyword persists custom classification", "[sentiment-db]") {
    const auto tempPath =
        (fs::temp_directory_path() / "feedback_analyzer_test_add_kw.db").string();
    fs::remove(tempPath);

    SentimentKeywordDb db(tempPath);
    REQUIRE(db.saveFromConstants());
    REQUIRE(db.addKeyword(u8"긍정", u8"슈퍼만족"));
    REQUIRE(SentimentClassifier::classifyUtf8(u8"슈퍼만족합니다") == u8"긍정");

    SentimentKeywordDb reload(tempPath);
    REQUIRE(reload.loadIntoConstants());
    REQUIRE(SentimentClassifier::classifyUtf8(u8"슈퍼만족합니다") == u8"긍정");

    fs::remove(tempPath);
}

TEST_CASE_METHOD(DomainFixture, "DB-03 removeKeyword drops match", "[sentiment-db]") {
    const auto tempPath =
        (fs::temp_directory_path() / "feedback_analyzer_test_rm_kw.db").string();
    fs::remove(tempPath);

    SentimentKeywordDb db(tempPath);
    Constants::SENTIMENT_KEYWORDS[u8"긍정"] = {u8"제거대상"};
    REQUIRE(db.saveFromConstants());
    REQUIRE(db.removeKeyword(u8"긍정", u8"제거대상"));
    REQUIRE(SentimentClassifier::classifyUtf8(u8"제거대상입니다") == u8"중립");

    fs::remove(tempPath);
}
