#include <catch2/catch_test_macros.hpp>

#include "DomainFixture.h"
#include "HttpFixture.h"
#include "Session.h"

TEST_CASE("HTTP-H01 POST analyze returns stats in HTML", "[p1][http][smoke]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks({});
    auto res = cli.Post("/analyze", "text=" + std::string(u8"배송이 빠르고 좋습니다"), "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(res->body.find("stats:") != std::string::npos);
    REQUIRE(res->body.find(u8"긍정") != std::string::npos);
}

TEST_CASE("HTTP-H02 POST filter with no data shows warning", "[p1][http][smoke]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks({});
    auto res = cli.Post("/filter",
                        "sentiment=" + std::string(u8"전체") + "&keyword=" + std::string(u8"전체"),
                        "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(res->body.find(u8"분석할 피드백이 없습니다") != std::string::npos);
}

TEST_CASE("HTTP-H03 filter then download CSV with BOM", "[p0][http][smoke]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks(makeFeedbacks({u8"배송이 빠르고 좋습니다"}));
    auto filterRes = cli.Post("/filter",
                              "sentiment=" + std::string(u8"긍정") + "&keyword=" + std::string(u8"전체"),
                              "application/x-www-form-urlencoded");
    REQUIRE(filterRes);
    REQUIRE(filterRes->status == 200);

    auto downRes = cli.Get("/download");
    REQUIRE(downRes);
    REQUIRE(downRes->status == 200);
    REQUIRE(downRes->body.substr(0, 3) == "\xEF\xBB\xBF");
    REQUIRE(downRes->body.find("text\n") != std::string::npos);
    REQUIRE(downRes->body.find(u8"배송이 빠르고 좋습니다") != std::string::npos);
}

TEST_CASE("HTTP-H04 POST upload skips header and increases rows", "[p1][http][smoke]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks({});
    const std::string csvBody = "text\n" + std::string(u8"업로드한문장") + "\n";
    httplib::UploadFormDataItems items = {
        {"file", csvBody, "test.csv", "text/csv"},
    };
    auto res = cli.Post("/upload", items);
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(Session::getCurrentFeedbacks().size() == 1);
    REQUIRE(Session::getCurrentFeedbacks()[0].getText() == u8"업로드한문장");
}
