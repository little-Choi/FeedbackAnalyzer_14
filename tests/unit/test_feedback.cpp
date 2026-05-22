#include <catch2/catch_test_macros.hpp>

#include "DomainFixture.h"

TEST_CASE("FB-01 Feedback getText returns stored text", "[p3][feedback]") {
    Feedback fb(u8"배송 빠름");
    REQUIRE(fb.getText() == u8"배송 빠름");
}

TEST_CASE("FB-02 empty string stores without throw", "[p2][feedback]") {
    REQUIRE_NOTHROW(Feedback(""));
    Feedback fb("");
    REQUIRE(fb.getText().empty());
}

TEST_CASE("FB-03 UTF-8 Korean bytes preserved", "[p2][feedback]") {
    const std::string text = u8"품질이 좋습니다";
    Feedback fb(text);
    REQUIRE(fb.getText() == text);
}

TEST_CASE("FB-04 long text stored and findable", "[p3][feedback]") {
    std::string longText(10 * 1024, 'a');
    longText += u8"배송";
    Feedback fb(longText);
    REQUIRE(fb.getText().size() == longText.size());
    REQUIRE(fb.getText().find(u8"배송") != std::string::npos);
}
