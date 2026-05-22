#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include "DomainFixture.h"
#include "TextAnalyzer.h"

namespace {

class CoutSilencer {
public:
    CoutSilencer() : previous_(std::cout.rdbuf(buffer_.rdbuf())) {}
    ~CoutSilencer() { std::cout.rdbuf(previous_); }

private:
    std::ostringstream buffer_;
    std::streambuf* previous_;
};

}  // namespace

TEST_CASE_METHOD(FiltersFixture, "FI-F01 positive filter all keyword", "[p0][filters]") {
    auto data = makeFeedbacks({u8"배송이 빠르고 좋습니다"});
    CoutSilencer silence;
    auto result = filters.fil(data, u8"긍정", u8"전체");
    REQUIRE(result.size() == 1);
}

TEST_CASE_METHOD(FiltersFixture, "FI-F02 all sentiment delivery sub-keyword", "[p0][filters]") {
    auto data = makeFeedbacks({u8"택배 배송"});
    CoutSilencer silence;
    auto result = filters.fil(data, u8"전체", u8"배송");
    REQUIRE(result.size() == 1);
}

TEST_CASE_METHOD(FiltersFixture, "FI-F03 positive and delivery AND", "[p1][filters]") {
    auto data = makeFeedbacks({u8"배송이 빠르고 좋습니다", u8"품질이 나쁩니다"});
    CoutSilencer silence;
    auto result = filters.fil(data, u8"긍정", u8"배송");
    REQUIRE(result.size() == 1);
}

TEST_CASE_METHOD(FiltersFixture, "FI-F04 empty data returns zero", "[p1][filters]") {
    std::vector<Feedback> empty;
    CoutSilencer silence;
    auto result = filters.fil(empty, u8"긍정", u8"배송");
    REQUIRE(result.empty());
}

TEST_CASE_METHOD(FiltersFixture, "FI-F05 quality filter matches main keyword", "[p0][filters][FI-F05]") {
    auto data = makeFeedbacks({u8"품질이 좋습니다"});
    CoutSilencer silence;
    auto result = filters.fil(data, u8"전체", u8"품질");
    REQUIRE(result.size() == 1);
}

TEST_CASE_METHOD(FiltersFixture, "FI-F06 neutral filter ok phrase matches stats", "[p0][filters][FI-F06]") {
    auto data = makeFeedbacks({u8"괜찮아요"});
    CoutSilencer silence;
    auto result = filters.fil(data, u8"중립", u8"전체");
    REQUIRE(result.size() == 1);
}

TEST_CASE_METHOD(FiltersFixture, "FI-F07 negative filter matches late keyword", "[p0][filters]") {
    auto data = makeFeedbacks({u8"배송이 늦다"});
    CoutSilencer silence;
    auto result = filters.fil(data, u8"부정", u8"전체");
    REQUIRE(result.size() == 1);
}

TEST_CASE_METHOD(FiltersFixture, "FI-F07b late past tense matches negative keyword", "[p0][filters][FI-F07b]") {
    auto data = makeFeedbacks({u8"배송이 늦었어요"});
    CoutSilencer silence;
    auto result = filters.fil(data, u8"부정", u8"전체");
    REQUIRE(result.size() == 1);
}

TEST_CASE_METHOD(FiltersFixture, "FI-P2-02 fil prints filtered lines to stdout", "[p2][filters]") {
    auto data = makeFeedbacks({u8"배송이 빠르고 좋습니다"});
    std::ostringstream buffer;
    auto* previous = std::cout.rdbuf(buffer.rdbuf());
    filters.fil(data, u8"긍정", u8"전체");
    std::cout.rdbuf(previous);
    REQUIRE(buffer.str().find(u8"배송이 빠르고 좋습니다") != std::string::npos);
}

TEST_CASE_METHOD(FiltersFixture, "FI-F08 sent positive count matches fil positive size", "[p2][filters]") {
    TextAnalyzer analyzer;
    auto data = makeFeedbacks({
        u8"배송이 빠르고 좋습니다",
        u8"품질이 나쁩니다",
        u8"그냥 무난한 하루",
    });
    auto sent = analyzer.sent(data);
    CoutSilencer silence;
    auto filtered = filters.fil(data, u8"긍정", u8"전체");
    REQUIRE(sent[u8"긍정"] == static_cast<int>(filtered.size()));
}
