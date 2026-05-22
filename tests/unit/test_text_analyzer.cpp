#include <catch2/catch_test_macros.hpp>

#include "DomainFixture.h"
#include "TextAnalyzer.h"

TEST_CASE_METHOD(TextAnalyzerFixture, "TA-S01 positive single feedback", "[p0][text_analyzer][sent]") {
    auto res = analyzer.sent(makeFeedbacks({u8"배송이 빠르고 좋습니다"}));
    REQUIRE(res[u8"긍정"] == 1);
    REQUIRE(res[u8"부정"] == 0);
}

TEST_CASE_METHOD(TextAnalyzerFixture, "TA-S02 negative feedback", "[p0][text_analyzer][sent]") {
    auto res = analyzer.sent(makeFeedbacks({u8"품질이 나쁘고 실망입니다"}));
    REQUIRE(res[u8"부정"] == 1);
}

TEST_CASE_METHOD(TextAnalyzerFixture, "TA-S03 neutral when no sentiment keyword", "[p0][text_analyzer][sent]") {
    auto res = analyzer.sent(makeFeedbacks({u8"그냥 무난한 하루"}));
    REQUIRE(res[u8"중립"] == 1);
}

TEST_CASE_METHOD(TextAnalyzerFixture, "TA-S04 mixed positive-negative counts as positive", "[p0][text_analyzer][sent][known-fail]") {
    auto res = analyzer.sent(makeFeedbacks({u8"좋은데 나쁜 부분도 있음"}));
    REQUIRE(res[u8"긍정"] == 1);
    REQUIRE(res[u8"부정"] == 0);
}

TEST_CASE_METHOD(TextAnalyzerFixture, "TA-S05 empty feedback list", "[p1][text_analyzer][sent]") {
    std::vector<Feedback> empty;
    auto res = analyzer.sent(empty);
    REQUIRE(res[u8"긍정"] == 0);
    REQUIRE(res[u8"중립"] == 0);
    REQUIRE(res[u8"부정"] == 0);
}

TEST_CASE_METHOD(TextAnalyzerFixture, "TA-S06 sentiment distribution sums to N", "[p1][text_analyzer][sent]") {
    auto data = makeFeedbacks({
        u8"배송이 빠르고 좋습니다",
        u8"품질이 나쁘고 실망입니다",
        u8"그냥 무난한 하루",
        u8"좋은데 나쁜 부분도 있음",
        u8"괜찮아요",
        u8"배송이 늦었어요",
        u8"가격이 저렴합니다",
        u8"서비스가 친절합니다",
        u8"사용이 편리합니다",
        u8"날씨가 맑다",
    });
    auto res = analyzer.sent(data);
    const int sum = res.at(u8"긍정") + res.at(u8"중립") + res.at(u8"부정");
    REQUIRE(sum == static_cast<int>(data.size()));
}

TEST_CASE_METHOD(TextAnalyzerFixture, "TA-S07 late delivery stats neutral", "[p0][text_analyzer][sent]") {
    auto res = analyzer.sent(makeFeedbacks({u8"배송이 늦었어요"}));
    REQUIRE(res[u8"중립"] == 1);
}

TEST_CASE_METHOD(TextAnalyzerFixture, "TA-S08 ok phrase stats neutral", "[p0][text_analyzer][sent]") {
    auto res = analyzer.sent(makeFeedbacks({u8"괜찮아요"}));
    REQUIRE(res[u8"중립"] == 1);
}

TEST_CASE_METHOD(TextAnalyzerFixture, "TA-K01 delivery keyword from main list", "[p0][text_analyzer][kw]") {
    auto res = analyzer.kw(makeFeedbacks({u8"택배 배송이 늦음"}));
    REQUIRE(res[u8"배송"] >= 1);
}

TEST_CASE_METHOD(TextAnalyzerFixture, "TA-K02 quality keyword", "[p0][text_analyzer][kw]") {
    auto res = analyzer.kw(makeFeedbacks({u8"품질이 좋습니다"}));
    REQUIRE(res[u8"품질"] >= 1);
}

TEST_CASE_METHOD(TextAnalyzerFixture, "TA-K03 multiple categories increment", "[p1][text_analyzer][kw]") {
    auto res = analyzer.kw(makeFeedbacks({u8"배송 빠르고 가격 저렴"}));
    REQUIRE(res[u8"배송"] >= 1);
    REQUIRE(res[u8"가격"] >= 1);
}

TEST_CASE_METHOD(TextAnalyzerFixture, "TA-K04 no category match", "[p2][text_analyzer][kw]") {
    auto res = analyzer.kw(makeFeedbacks({u8"날씨가 맑다"}));
    for (const auto& entry : res) {
        REQUIRE(entry.second == 0);
    }
}

TEST_CASE_METHOD(TextAnalyzerFixture, "TA-K05 delivery delay token matches main kw", "[p1][text_analyzer][kw]") {
    auto res = analyzer.kw(makeFeedbacks({u8"배송지연"}));
    REQUIRE(res[u8"배송"] == 1);
}
