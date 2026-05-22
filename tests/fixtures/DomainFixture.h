#pragma once

#include "Constants.h"
#include "Feedback.h"
#include "FeedbackRepository.h"
#include "Filters.h"
#include "Session.h"
#include "TextAnalyzer.h"

#include <initializer_list>
#include <string>
#include <vector>

struct DomainFixture {
    DomainFixture() {
        Constants::init();
        FeedbackRepository::init();
        Session::updateCurrentFeedbacks({});
        FeedbackRepository::clearLastFiltered();
    }

    ~DomainFixture() { Session::updateCurrentFeedbacks({}); }
};

struct TextAnalyzerFixture : DomainFixture {
    TextAnalyzer analyzer;
};

struct FiltersFixture : DomainFixture {
    Filters filters;
};

struct SessionFixture : DomainFixture {};

inline std::vector<Feedback> makeFeedbacks(std::initializer_list<std::string> texts) {
    std::vector<Feedback> out;
    for (const auto& t : texts) {
        out.emplace_back(t);
    }
    return out;
}
