#include <catch2/catch_test_macros.hpp>

#include "HttpFixture.h"
#include "Session.h"

TEST_CASE("HTTP-TR01 upload date,text CSV sets trend flag", "[p1][http][trend]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks({});
    const std::string csvBody = std::string(u8"date,text\n2026-05-01,") + u8"배송이 빠르고 좋습니다\n";
    httplib::UploadFormDataItems items = {
        {"file", csvBody, "test_feedback_trend.csv", "text/csv"},
    };
    auto res = cli.Post("/upload", items);
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(res->body.find("trend:yes") != std::string::npos);
    REQUIRE(Session::getCurrentFeedbacks().size() == 1);
    REQUIRE(Session::getCurrentFeedbacks()[0].hasDate());
}
