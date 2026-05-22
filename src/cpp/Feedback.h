#pragma once

#include <string>
#include <utility>

class Feedback {
    std::string text;
    std::string date;

public:
    Feedback(std::string t, std::string d = "")
        : text(std::move(t)), date(std::move(d)) {}

    const std::string& getText() const { return text; }
    const std::string& getDate() const { return date; }
    [[nodiscard]] bool hasDate() const { return !date.empty(); }
};
