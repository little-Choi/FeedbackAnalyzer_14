#pragma once

#include "Constants.h"
#include "Feedback.h"
#include "Filters.h"
#include "httplib.h"
#include "Session.h"
#include "TextAnalyzer.h"

#include <chrono>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

// In-process server mirroring main.cpp routes (legacy main.cpp unchanged).
class HttpTestServer {
public:
    static constexpr const char* kThrowMarker = "__TEST_THROW__";

    static std::vector<Feedback>& filData() { return fil_data_; }

    HttpTestServer() {
        Constants::init();
        Filters::initFilterKeywords();
        Session::updateCurrentFeedbacks({});
        fil_data_.clear();

        svr_.Get("/", [](const httplib::Request&, httplib::Response& res) {
            auto& feedbacks = Session::getOldDataFromSession("current_feedbacks");
            std::string html = "<html>size:" + std::to_string(feedbacks.size()) + "</html>";
            res.set_content(html, "text/html; charset=UTF-8");
        });

        svr_.Post("/analyze", [this](const httplib::Request& req, httplib::Response& res) {
            try {
                if (req.body.find(kThrowMarker) != std::string::npos) {
                    throw std::runtime_error("test analyze exception");
                }
                auto& feedbacks = Session::getCurrentFeedbacks();
                auto params = parseForm(req.body);
                std::string text = params["text"];
                if (!text.empty()) {
                    feedbacks.emplace_back(text);
                }
                std::map<std::string, int> sentimentResults;
                std::map<std::string, int> keywordResults;
                if (!feedbacks.empty()) {
                    sentimentResults = analyzer_.sent(feedbacks);
                    keywordResults = analyzer_.kw(feedbacks);
                }
                res.status = 200;
                res.set_content(buildStatsHtml("analyze", sentimentResults, keywordResults),
                                "text/html; charset=UTF-8");
            } catch (const std::exception&) {
                res.status = 200;
                res.set_content(u8"<html>error:처리 중 오류가 발생했습니다.</html>",
                                "text/html; charset=UTF-8");
            }
        });

        svr_.Post("/upload", [this](const httplib::Request& req, httplib::Response& res) {
            try {
                if (req.body.find(kThrowMarker) != std::string::npos) {
                    throw std::runtime_error("test upload exception");
                }
                auto& feedbacks = Session::getCurrentFeedbacks();
                const size_t before = feedbacks.size();
                if (req.form.has_file("file")) {
                    const auto file = req.form.get_file("file");
                    if (!file.content.empty()) {
                        std::istringstream stream(file.content);
                        std::string line;
                        bool firstLine = true;
                        int textColumnIndex = 0;
                        while (std::getline(stream, line)) {
                            if (!line.empty() && line.back() == '\r') {
                                line.pop_back();
                            }
                            if (line.empty()) {
                                continue;
                            }
                            auto fields = parseCsvLine(line);
                            if (fields.empty()) {
                                continue;
                            }
                            if (firstLine) {
                                firstLine = false;
                                if (rowLooksLikeHeader(fields)) {
                                    textColumnIndex = findTextColumnIndex(fields);
                                    continue;
                                }
                            }
                            if (textColumnIndex >= 0 &&
                                static_cast<size_t>(textColumnIndex) < fields.size() &&
                                !fields[static_cast<size_t>(textColumnIndex)].empty()) {
                                feedbacks.emplace_back(
                                    fields[static_cast<size_t>(textColumnIndex)]);
                            }
                        }
                    }
                }
                const size_t added = feedbacks.size() - before;
                std::map<std::string, int> sentimentResults;
                std::map<std::string, int> keywordResults;
                if (!feedbacks.empty()) {
                    sentimentResults = analyzer_.sent(feedbacks);
                    keywordResults = analyzer_.kw(feedbacks);
                }
                std::ostringstream html;
                html << "<html>uploaded:" << feedbacks.size() << ";added:" << added;
                html << ";stats:";
                for (const auto& e : sentimentResults) {
                    html << e.first << "=" << e.second << ";";
                }
                for (const auto& e : keywordResults) {
                    html << e.first << "=" << e.second << ";";
                }
                html << "</html>";
                res.status = 200;
                res.set_content(html.str(), "text/html; charset=UTF-8");
            } catch (const std::exception&) {
                res.status = 200;
                res.set_content(u8"<html>error:파일 업로드 중 오류가 발생했습니다.</html>",
                                "text/html; charset=UTF-8");
            }
        });

        svr_.Post("/filter", [this](const httplib::Request& req, httplib::Response& res) {
            try {
                if (req.body.find(kThrowMarker) != std::string::npos) {
                    throw std::runtime_error("test filter exception");
                }
                auto& feedbacks = Session::getCurrentFeedbacks();
                auto params = parseForm(req.body);
                std::string sentiment = params["sentiment"];
                std::string keyword = params["keyword"];
                if (feedbacks.empty()) {
                    res.status = 200;
                    res.set_content(u8"<html>warning:분석할 피드백이 없습니다.</html>",
                                    "text/html; charset=UTF-8");
                    return;
                }
                auto filtered = filters_.fil(feedbacks, sentiment, keyword);
                if (filtered.empty()) {
                    res.status = 200;
                    res.set_content(u8"<html>warning:필터링 결과가 없습니다.</html>",
                                    "text/html; charset=UTF-8");
                    return;
                }
                fil_data_ = filtered;
                auto sentimentResults = analyzer_.sent(filtered);
                auto keywordResults = analyzer_.kw(filtered);
                res.status = 200;
                res.set_content(buildStatsHtml("filtered:" + std::to_string(filtered.size()),
                                               sentimentResults, keywordResults),
                                "text/html; charset=UTF-8");
            } catch (const std::exception&) {
                res.status = 200;
                res.set_content(u8"<html>error:처리 중 오류가 발생했습니다.</html>",
                                "text/html; charset=UTF-8");
            }
        });

        svr_.Get("/download", [](const httplib::Request&, httplib::Response& res) {
            std::ostringstream csv;
            csv << "\xEF\xBB\xBF";
            csv << "text\n";
            for (const auto& iter : fil_data_) {
                csv << iter.getText() << "\n";
            }
            res.set_header("Content-Disposition", "attachment; filename=\"filtered_feedback.csv\"");
            res.status = 200;
            res.set_content(csv.str(), "text/csv; charset=UTF-8");
        });

        const int port = svr_.bind_to_any_port("127.0.0.1");
        base_url_ = "http://127.0.0.1:" + std::to_string(port);
        thread_ = std::thread([this]() { svr_.listen_after_bind(); });
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    ~HttpTestServer() {
        svr_.stop();
        if (thread_.joinable()) {
            thread_.join();
        }
        Session::updateCurrentFeedbacks({});
        fil_data_.clear();
    }

    const std::string& baseUrl() const { return base_url_; }

private:
    static std::vector<Feedback> fil_data_;
    httplib::Server svr_;
    std::thread thread_;
    std::string base_url_;
    TextAnalyzer analyzer_;
    Filters filters_;

    static std::string buildStatsHtml(const std::string& prefix,
                                      const std::map<std::string, int>& sentimentResults,
                                      const std::map<std::string, int>& keywordResults) {
        std::ostringstream html;
        html << "<html>" << prefix << ";stats:";
        for (const auto& e : sentimentResults) {
            html << e.first << "=" << e.second << ";";
        }
        for (const auto& e : keywordResults) {
            html << e.first << "=" << e.second << ";";
        }
        html << "</html>";
        return html.str();
    }

    static std::string urlDecode(const std::string& str) {
        std::string result;
        for (size_t i = 0; i < str.size(); i++) {
            if (str[i] == '%' && i + 2 < str.size()) {
                int val = 0;
                std::istringstream iss(str.substr(i + 1, 2));
                if (iss >> std::hex >> val) {
                    result += static_cast<char>(val);
                    i += 2;
                } else {
                    result += str[i];
                }
            } else if (str[i] == '+') {
                result += ' ';
            } else {
                result += str[i];
            }
        }
        return result;
    }

    static std::map<std::string, std::string> parseForm(const std::string& body) {
        std::map<std::string, std::string> params;
        std::istringstream stream(body);
        std::string pair;
        while (std::getline(stream, pair, '&')) {
            auto eq = pair.find('=');
            if (eq != std::string::npos) {
                params[urlDecode(pair.substr(0, eq))] = urlDecode(pair.substr(eq + 1));
            }
        }
        return params;
    }

    static int findTextColumnIndex(const std::vector<std::string>& fields) {
        for (size_t i = 0; i < fields.size(); ++i) {
            if (fields[i] == "text") {
                return static_cast<int>(i);
            }
        }
        return 0;
    }

    static bool rowLooksLikeHeader(const std::vector<std::string>& fields) {
        for (const auto& f : fields) {
            if (f == "text" || f == "id") {
                return true;
            }
        }
        return false;
    }

    static std::vector<std::string> parseCsvLine(const std::string& line) {
        std::vector<std::string> fields;
        std::string field;
        bool inQuotes = false;
        for (size_t i = 0; i < line.size(); i++) {
            char c = line[i];
            if (c == '"') {
                inQuotes = !inQuotes;
            } else if (c == ',' && !inQuotes) {
                fields.push_back(field);
                field.clear();
            } else {
                field += c;
            }
        }
        fields.push_back(field);
        return fields;
    }
};

inline std::vector<Feedback> HttpTestServer::fil_data_;
