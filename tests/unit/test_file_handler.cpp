#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include "DomainFixture.h"
#include "FileHandler.h"

TEST_CASE("FH-01 saveResult prints legacy stub output", "[p2][file_handler]") {
    std::ostringstream buffer;
    auto* previous = std::cout.rdbuf(buffer.rdbuf());
    FileHandler handler;
    handler.saveResult(makeFeedbacks({u8"저장테스트"}));
    std::cout.rdbuf(previous);
    REQUIRE(buffer.str().find("saveResult") != std::string::npos);
    REQUIRE(buffer.str().find(u8"저장테스트") != std::string::npos);
}

TEST_CASE("FH-02 save delegates to saveResult", "[p3][file_handler]") {
    std::ostringstream buffer;
    auto* previous = std::cout.rdbuf(buffer.rdbuf());
    FileHandler handler;
    handler.save(makeFeedbacks({u8"위임"}));
    std::cout.rdbuf(previous);
    REQUIRE(buffer.str().find("saveResult1") != std::string::npos);
}
