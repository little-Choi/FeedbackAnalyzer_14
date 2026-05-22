#include <catch2/catch_test_macros.hpp>

#include "DomainFixture.h"
#include "LegacyCsv.h"
#include "SentimentMatcher.h"
#include "TextAnalyzer.h"

TEST_CASE("P0 regression bundle CSV-R02 FI-F05 FI-F06 FI-F07b TA-S07", "[p0][regression]") {
    Constants::init();
    Filters::initFilterKeywords();

    const std::string csv = std::string(u8"id,text\n1,") + u8"실제본문\n";
    auto texts = csvUploadTexts(csv);
    INFO("CSV-R02 parsed count=" << texts.size());
    REQUIRE(texts.size() == 1);
    REQUIRE(texts[0] == u8"실제본문");

    TextAnalyzer analyzer;
    Filters filters;

    const auto qualityData = makeFeedbacks({u8"품질이 좋습니다"});
    const auto okData = makeFeedbacks({u8"괜찮아요"});
    const auto lateData = makeFeedbacks({u8"배송이 늦었어요"});

    REQUIRE(classifySentiment(u8"품질이 좋습니다") == u8"긍정");
    REQUIRE(classifySentiment(u8"괜찮아요") == u8"중립");
    REQUIRE(classifySentiment(u8"배송이 늦었어요") == u8"부정");

    auto f05 = filters.fil(qualityData, u8"전체", u8"품질");
    INFO("FI-F05 size=" << f05.size());
    REQUIRE(f05.size() == 1);

    auto f06 = filters.fil(okData, u8"중립", u8"전체");
    INFO("FI-F06 size=" << f06.size());
    REQUIRE(f06.size() == 1);

    auto f07b = filters.fil(lateData, u8"부정", u8"전체");
    INFO("FI-F07b size=" << f07b.size());
    REQUIRE(f07b.size() == 1);

    auto sent = analyzer.sent(lateData);
    INFO("TA-S07 neg=" << sent[u8"부정"] << " neu=" << sent[u8"중립"]);
    REQUIRE(sent[u8"부정"] == 1);
    REQUIRE(sent[u8"중립"] == 0);
}
