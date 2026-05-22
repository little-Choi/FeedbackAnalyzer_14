#include <catch2/catch_test_macros.hpp>

#include "DomainFixture.h"
#include "LegacyCsv.h"

TEST_CASE("CSV-R02 id,text uses text column", "[p0][csv]") {
    const std::string csv = "id,text\n1,실제본문\n";
    auto texts = csvUploadTexts(csv);
    REQUIRE(texts.size() == 1);
    REQUIRE(texts[0] == u8"실제본문");
}

TEST_CASE("CSV-R04 download without filter yields BOM header only", "[p0][csv]") {
    std::vector<Feedback> emptyFilData;
    const std::string csv = buildDownloadCsv(emptyFilData);
    REQUIRE(csv.size() >= 3);
    REQUIRE(csv.substr(0, 3) == "\xEF\xBB\xBF");
    REQUIRE(csv.find("text\n") != std::string::npos);
    const auto afterHeader = csv.find('\n', csv.find("text\n"));
    if (afterHeader != std::string::npos && afterHeader + 1 < csv.size()) {
        FAIL("Expected no data rows when fil_data empty");
    }
}

TEST_CASE("CSV-ESC comma in text is not escaped on download", "[p1][csv]") {
    auto csv = buildDownloadCsv(makeFeedbacks({u8"배송, 좋아요"}));
    REQUIRE(csv.find(u8"배송, 좋아요") != std::string::npos);
}

TEST_CASE("parseCsvLine splits quoted comma", "[p2][csv]") {
    auto fields = parseCsvLine(R"("a,b",c)");
    REQUIRE(fields.size() == 2);
    REQUIRE(fields[0] == "a,b");
    REQUIRE(fields[1] == "c");
}

TEST_CASE("CSV upload skips header row", "[p1][csv]") {
    const std::string csv = "text\n" + std::string(u8"첫데이터") + "\n" + u8"둘째데이터\n";
    auto texts = csvUploadTexts(csv);
    REQUIRE(texts.size() == 2);
    REQUIRE(texts[0] == u8"첫데이터");
}

TEST_CASE("CSV upload header only adds zero rows", "[p2][csv]") {
    const std::string csv = "text\n";
    auto texts = csvUploadTexts(csv);
    REQUIRE(texts.empty());
}

TEST_CASE("CSV-NH01 no header row keeps data line", "[p1][csv]") {
    const std::string csv = std::string(u8"단일데이터") + "\n";
    auto texts = csvUploadTexts(csv);
    REQUIRE(texts.size() == 1);
    REQUIRE(texts[0] == u8"단일데이터");
}
