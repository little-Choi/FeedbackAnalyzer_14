#include <catch2/catch_test_macros.hpp>

#include <httplib.h>
#include <sstream>

#include "DomainFixture.h"
#include "GoldenMaster.h"
#include "GoldenSerialize.h"
#include "HttpFixture.h"
#include "LegacyCsv.h"
#include "Session.h"
#include "TextAnalyzer.h"

namespace {

std::string captureFilStdout(Filters& filters,
                             const std::vector<Feedback>& data,
                             const std::string& sentiment,
                             const std::string& keyword) {
    std::ostringstream buffer;
    auto* previous = std::cout.rdbuf(buffer.rdbuf());
    filters.fil(data, sentiment, keyword);
    std::cout.rdbuf(previous);
    return buffer.str();
}

}  // namespace

TEST_CASE("GM-TA sent distribution three feedbacks", "[golden][regression][text_analyzer]") {
    TextAnalyzerFixture fixture;
    auto data = makeFeedbacks({
        u8"배송이 빠르고 좋습니다",
        u8"품질이 나쁘고 실망입니다",
        u8"그냥 무난한 하루",
    });
    std::ostringstream out;
    out << serializeSentimentMap(fixture.analyzer.sent(data));
    out << serializeKeywordMap(fixture.analyzer.kw(data));
    golden::assertMatchesGolden("domain/TA_sent_kw_three_feedbacks.txt", out.str());
}

TEST_CASE("GM-FI fil stdout positive filter", "[golden][regression][filters]") {
    FiltersFixture fixture;
    auto data = makeFeedbacks({u8"배송이 빠르고 좋습니다", u8"품질이 나쁩니다"});
    const std::string stdoutCapture = captureFilStdout(fixture.filters, data, u8"긍정", u8"전체");
    golden::assertMatchesGolden("domain/FI_fil_stdout_positive.txt", stdoutCapture);
}

TEST_CASE("GM-CSV empty download", "[golden][regression][csv]") {
    DomainFixture fixture;
    (void)fixture;
    const std::string csv = buildDownloadCsv({});
    golden::assertMatchesGolden("csv/R04_empty_download.csv", csv);
}

TEST_CASE("GM-CSV filtered download one row", "[golden][regression][csv]") {
    DomainFixture fixture;
    (void)fixture;
    const std::string csv = buildDownloadCsv(makeFeedbacks({u8"배송이 빠르고 좋습니다"}));
    golden::assertMatchesGolden("csv/H03_filtered_download.csv", csv);
}

TEST_CASE("GM-CSV upload id,text parsed texts", "[golden][regression][csv]") {
    DomainFixture fixture;
    (void)fixture;
    const std::string csv = std::string(u8"id,text\n1,") + u8"실제본문\n";
    golden::assertMatchesGolden("csv/R02_upload_parsed.txt", serializeTextLines(csvUploadTexts(csv)));
}

TEST_CASE("GM-HTTP H01 analyze response body", "[golden][regression][http]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks({});
    auto res = cli.Post("/analyze",
                        "text=" + std::string(u8"배송이 빠르고 좋습니다"),
                        "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    golden::assertMatchesGolden("http/H01_analyze_positive.html", res->body);
}

TEST_CASE("GM-HTTP H02 filter no data warning", "[golden][regression][http]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks({});
    auto res = cli.Post("/filter",
                        "sentiment=" + std::string(u8"전체") + "&keyword=" + std::string(u8"전체"),
                        "application/x-www-form-urlencoded");
    REQUIRE(res);
    REQUIRE(res->status == 200);
    golden::assertMatchesGolden("http/H02_filter_no_data.html", res->body);
}

TEST_CASE("GM-HTTP H03 filter then download", "[golden][regression][http]") {
    HttpTestServer server;
    httplib::Client cli(server.baseUrl());
    Session::updateCurrentFeedbacks(makeFeedbacks({u8"배송이 빠르고 좋습니다"}));
    auto filterRes = cli.Post("/filter",
                              "sentiment=" + std::string(u8"긍정") + "&keyword=" + std::string(u8"전체"),
                              "application/x-www-form-urlencoded");
    REQUIRE(filterRes);
    REQUIRE(filterRes->status == 200);
    golden::assertMatchesGolden("http/H03_filter_stats.html", filterRes->body);

    auto downRes = cli.Get("/download");
    REQUIRE(downRes);
    REQUIRE(downRes->status == 200);
    golden::assertMatchesGolden("http/H03_download.csv", downRes->body);
}

TEST_CASE("GM-HTTP H04 upload response", "[golden][regression][http]") {
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
    golden::assertMatchesGolden("http/H04_upload_stats.html", res->body);
}
