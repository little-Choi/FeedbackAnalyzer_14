#include <catch2/catch_test_macros.hpp>

#include "DomainFixture.h"
#include "Session.h"

TEST_CASE_METHOD(SessionFixture, "SE-R01 updateCurrentFeedbacks grows size", "[p1][session]") {
    Session::updateCurrentFeedbacks(makeFeedbacks({u8"첫번째"}));
    REQUIRE(Session::getCurrentFeedbacks().size() == 1);
    Session::updateCurrentFeedbacks(makeFeedbacks({u8"첫번째", u8"두번째"}));
    REQUIRE(Session::getCurrentFeedbacks().size() == 2);
}

TEST_CASE_METHOD(SessionFixture, "SE-R02 getOldDataFromSession ignores key", "[p2][session]") {
    Session::updateCurrentFeedbacks(makeFeedbacks({u8"키무시"}));
    auto& a = Session::getOldDataFromSession("any");
    auto& b = Session::getOldDataFromSession("other");
    REQUIRE(&a == &b);
    REQUIRE(a.size() == 1);
}

TEST_CASE_METHOD(SessionFixture, "SE-R03 returned reference reflects external mutation", "[p1][session]") {
    Session::updateCurrentFeedbacks({});
    auto& data = Session::getCurrentFeedbacks();
    data.emplace_back(u8"외부추가");
    REQUIRE(Session::getCurrentFeedbacks().size() == 1);
}

TEST_CASE_METHOD(SessionFixture, "SE-R04 fixture clears static session between cases", "[p0][session]") {
    Session::updateCurrentFeedbacks(makeFeedbacks({u8"누적테스트"}));
    REQUIRE(Session::getCurrentFeedbacks().size() == 1);
}
