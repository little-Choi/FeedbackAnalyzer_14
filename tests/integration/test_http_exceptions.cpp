#include <catch2/catch_test_macros.hpp>

#include "DomainFixture.h"
#include "HttpFixture.h"
#include "Session.h"

TEST_CASE("EX-01 analyze handler returns error HTML on exception", "[p2][http][exception]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks({});
    auto res = cli.Post("/analyze",
                        std::string("text=x&") + HttpTestServer::kThrowMarker,
                        "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(res->body.find(u8"처리 중 오류가 발생했습니다") != std::string::npos);
}

TEST_CASE("EX-02 upload empty file adds zero rows", "[p2][http][exception]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks(makeFeedbacks({u8"기존데이터"}));
    const size_t before = Session::getCurrentFeedbacks().size();
    httplib::UploadFormDataItems items = {
        {"file", "", "empty.csv", "text/csv"},
    };
    auto res = cli.Post("/upload", items);
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(Session::getCurrentFeedbacks().size() == before);
    REQUIRE(res->body.find("added:0") != std::string::npos);
}

TEST_CASE("EX-03 upload without file field keeps feedbacks", "[p2][http][exception]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks(makeFeedbacks({u8"유지"}));
    auto res = cli.Post("/upload", "", "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(Session::getCurrentFeedbacks().size() == 1);
    REQUIRE(Session::getCurrentFeedbacks()[0].getText() == u8"유지");
}

TEST_CASE("EX-04 filter handler returns error HTML on exception", "[p2][http][exception]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks(makeFeedbacks({u8"배송이 빠르고 좋습니다"}));
    auto res = cli.Post("/filter",
                        std::string("sentiment=") + u8"전체&keyword=" + u8"전체&" +
                            HttpTestServer::kThrowMarker,
                        "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(res->body.find(u8"처리 중 오류가 발생했습니다") != std::string::npos);
}

TEST_CASE("EX-05 invalid sentiment filter yields warning", "[p2][http][exception]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks(makeFeedbacks({u8"배송이 빠르고 좋습니다"}));
    auto res = cli.Post("/filter",
                        "sentiment=invalid&keyword=" + std::string(u8"전체"),
                        "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    REQUIRE(res->body.find(u8"필터링 결과가 없습니다") != std::string::npos);
}

TEST_CASE("HTTP-H04b upload response has empty stats like legacy", "[p1][http][known-fail]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks({});
    const std::string csvBody = "text\n" + std::string(u8"업로드행") + "\n";
    httplib::UploadFormDataItems items = {
        {"file", csvBody, "test.csv", "text/csv"},
    };
    auto res = cli.Post("/upload", items);
    REQUIRE(res);
    REQUIRE(res->status == 200);
    const auto statsPos = res->body.find("stats:");
    REQUIRE(statsPos != std::string::npos);
    const auto afterStats = res->body.substr(statsPos + 6);
    REQUIRE(afterStats.find(u8"긍정=") == std::string::npos);
    REQUIRE(afterStats.find(u8"배송=") == std::string::npos);
}
