#pragma once

#include "FeedbackRepository.h"
#include "Feedback.h"

#include <string>
#include <vector>

// Thin legacy facade over FeedbackRepository (Phase 4-1).
class Session {
public:
    static void initSessionStateUgly() { FeedbackRepository::init(); }

    static std::vector<Feedback>& getOldDataFromSession(const std::string& /*key*/) {
        return FeedbackRepository::all();
    }

    static void updateCurrentFeedbacks(const std::vector<Feedback>& feedbacks) {
        FeedbackRepository::setAll(feedbacks);
    }

    static std::vector<Feedback>& getCurrentFeedbacks() { return FeedbackRepository::all(); }
};
