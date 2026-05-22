#include <catch2/catch_test_macros.hpp>

#include <chrono>

#include "DomainFixture.h"
#include "TextAnalyzer.h"

TEST_CASE_METHOD(TextAnalyzerFixture, "PERF-01 sent and kw handle 1000 feedbacks", "[perf][performance]") {
    std::vector<Feedback> data;
    data.reserve(1000);
    for (int i = 0; i < 1000; ++i) {
        switch (i % 3) {
            case 0:
                data.emplace_back(u8"배송이 빠르고 좋습니다");
                break;
            case 1:
                data.emplace_back(u8"품질이 나쁘고 실망입니다");
                break;
            default:
                data.emplace_back(u8"그냥 무난한 하루");
                break;
        }
    }

    const auto start = std::chrono::steady_clock::now();
    const auto sent = analyzer.sent(data);
    const auto kw = analyzer.kw(data);
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start)
            .count();

    const int sum = sent.at(u8"긍정") + sent.at(u8"중립") + sent.at(u8"부정");
    REQUIRE(sum == 1000);
    int kwTotal = 0;
    for (const auto& entry : kw) {
        kwTotal += entry.second;
    }
    REQUIRE(kwTotal >= 0);
    REQUIRE(elapsed < 10000);
}
