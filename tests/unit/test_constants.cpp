#include <catch2/catch_test_macros.hpp>

#include "Constants.h"
#include "DomainFixture.h"

TEST_CASE("CO-01 Constants::init is idempotent", "[p2][constants]") {
    REQUIRE_NOTHROW(Constants::init());
    REQUIRE_NOTHROW(Constants::init());
}

TEST_CASE_METHOD(DomainFixture, "CO-02 five category keys exist", "[p2][constants]") {
    const std::vector<std::string> expected = {
        u8"배송", u8"품질", u8"가격", u8"서비스", u8"사용성"};
    for (const auto& cat : expected) {
        REQUIRE(Constants::CATEGORY_KEYWORDS.count(cat) > 0);
    }
}

TEST_CASE_METHOD(DomainFixture, "CO-03 each category has main keywords", "[p0][constants]") {
    for (const auto& entry : Constants::CATEGORY_KEYWORDS) {
        REQUIRE(entry.second.count("main") > 0);
        REQUIRE_FALSE(entry.second.at("main").empty());
    }
}

TEST_CASE_METHOD(DomainFixture, "CO-04 positive sentiment allows duplicates", "[p3][constants]") {
    const auto& positive = Constants::SENTIMENT_KEYWORDS[u8"긍정"];
    REQUIRE(positive.size() > 1);
    bool hasDuplicate = false;
    for (size_t i = 0; i < positive.size() && !hasDuplicate; ++i) {
        for (size_t j = i + 1; j < positive.size(); ++j) {
            if (positive[i] == positive[j]) {
                hasDuplicate = true;
                break;
            }
        }
    }
    REQUIRE(hasDuplicate);
}
