#pragma once

#include "Feedback.h"

#include <vector>

class FeedbackRepository {
public:
    static void init() {}

    [[nodiscard]] static std::vector<Feedback>& all() { return storage_; }

    [[nodiscard]] static std::vector<Feedback>& lastFiltered() { return lastFiltered_; }

    static void setAll(const std::vector<Feedback>& feedbacks) { storage_ = feedbacks; }

    static void setLastFiltered(const std::vector<Feedback>& filtered) {
        lastFiltered_ = filtered;
    }

    static void clearLastFiltered() { lastFiltered_.clear(); }

private:
    static std::vector<Feedback> storage_;
    static std::vector<Feedback> lastFiltered_;
};
