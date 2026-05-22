#pragma once

#include "Constants.h"

#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class SentimentKeywordDb {
public:
    explicit SentimentKeywordDb(std::string path) : path_(std::move(path)) {}

    [[nodiscard]] const std::string& path() const { return path_; }

    bool loadIntoConstants() {
        std::ifstream in(path_);
        if (!in) {
            return false;
        }
        std::map<std::string, std::vector<std::string>> loaded;
        loaded[Constants::SENTIMENT_POSITIVE] = {};
        loaded[Constants::SENTIMENT_NEGATIVE] = {};
        loaded[Constants::SENTIMENT_NEUTRAL] = {};

        std::string line;
        while (std::getline(in, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.empty() || line[0] == '#') {
                continue;
            }
            const auto tab = line.find('\t');
            if (tab == std::string::npos) {
                continue;
            }
            const std::string sentiment = line.substr(0, tab);
            const std::string keyword = line.substr(tab + 1);
            if (keyword.empty()) {
                continue;
            }
            if (sentiment == Constants::SENTIMENT_POSITIVE ||
                sentiment == Constants::SENTIMENT_NEGATIVE ||
                sentiment == Constants::SENTIMENT_NEUTRAL) {
                loaded[sentiment].push_back(keyword);
            }
        }
        if (loaded[Constants::SENTIMENT_POSITIVE].empty() &&
            loaded[Constants::SENTIMENT_NEGATIVE].empty()) {
            return false;
        }
        Constants::SENTIMENT_KEYWORDS = std::move(loaded);
        return true;
    }

    bool saveFromConstants() const {
        std::ofstream out(path_, std::ios::trunc);
        if (!out) {
            return false;
        }
        out << "# sentiment keyword database (tab-separated)\n";
        for (const auto& entry : Constants::SENTIMENT_KEYWORDS) {
            for (const auto& kw : entry.second) {
                out << entry.first << '\t' << kw << '\n';
            }
        }
        return static_cast<bool>(out);
    }

    bool addKeyword(const std::string& sentiment, const std::string& keyword) {
        if (keyword.empty()) {
            return false;
        }
        auto& list = Constants::SENTIMENT_KEYWORDS[sentiment];
        for (const auto& existing : list) {
            if (existing == keyword) {
                return saveFromConstants();
            }
        }
        list.push_back(keyword);
        return saveFromConstants();
    }

    bool removeKeyword(const std::string& sentiment, const std::string& keyword) {
        auto it = Constants::SENTIMENT_KEYWORDS.find(sentiment);
        if (it == Constants::SENTIMENT_KEYWORDS.end()) {
            return false;
        }
        auto& list = it->second;
        const auto pos = std::find(list.begin(), list.end(), keyword);
        if (pos == list.end()) {
            return false;
        }
        list.erase(pos);
        return saveFromConstants();
    }

    [[nodiscard]] std::map<std::string, std::vector<std::string>> snapshot() const {
        return Constants::SENTIMENT_KEYWORDS;
    }

private:
    std::string path_;
};
