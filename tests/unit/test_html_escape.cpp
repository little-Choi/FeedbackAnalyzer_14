#include <catch2/catch_test_macros.hpp>

#include "LegacyHtml.h"

TEST_CASE("HTML-01 escapeHtml encodes special chars", "[p2][html]") {
    REQUIRE(escapeHtml("<a&b>") == "&lt;a&amp;b&gt;");
    REQUIRE(escapeHtml("\"quote\"") == "&quot;quote&quot;");
}

TEST_CASE("HTML-02 apostrophe not escaped in legacy", "[p3][html][known-fail]") {
    REQUIRE(escapeHtml("it's") == "it's");
}
