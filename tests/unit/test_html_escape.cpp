#include <catch2/catch_test_macros.hpp>

#include "LegacyHtml.h"

TEST_CASE("HTML-01 escapeHtml encodes special chars", "[p2][html]") {
    REQUIRE(escapeHtml("<a&b>") == "&lt;a&amp;b&gt;");
    REQUIRE(escapeHtml("\"quote\"") == "&quot;quote&quot;");
}

TEST_CASE("HTML-02 apostrophe is escaped", "[p3][html]") {
    REQUIRE(escapeHtml("it's") == "it&#39;s");
}
