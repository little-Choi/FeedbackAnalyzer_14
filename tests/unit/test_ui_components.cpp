#include <catch2/catch_test_macros.hpp>

#include "UIComponents.h"

TEST_CASE("UI-01 category list golden", "[p2][ui]") {
    const std::vector<std::string> expected = {
        u8"배송", u8"품질", u8"가격", u8"서비스", u8"사용성"};
    const auto& cats = UIComponents::getCategories();
    REQUIRE(cats == expected);
}

TEST_CASE("UI-02 categories count is five", "[p3][ui]") {
    REQUIRE(UIComponents::getCategories().size() == 5);
}
