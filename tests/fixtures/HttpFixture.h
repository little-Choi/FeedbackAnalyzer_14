#pragma once

#include "AppConfig.h"
#include "Constants.h"
#include "CsvHelpers.h"
#include "Feedback.h"
#include "FeedbackRepository.h"
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

    static std::vector<Feedback>& filData() { return FeedbackRepository::lastFiltered(); }

    HttpTestServer() {
        Constants::init();
        FeedbackRepository::init();
        Session::updateCurrentFeedbacks({});
        FeedbackRepository::clearLastFiltered();

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
                        appendFeedbackFromCsvContent(file.content, feedbacks);
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
                FeedbackRepository::setLastFiltered(filtered);
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
            const std::string csv = buildDownloadCsv(FeedbackRepository::lastFiltered());
            res.set_header("Content-Disposition", "attachment; filename=\"filtered_feedback.csv\"");
            res.status = 200;
            res.set_content(csv, "text/csv; charset=UTF-8");
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
        FeedbackRepository::clearLastFiltered();
    }

    const std::string& baseUrl() const { return base_url_; }

private:
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

};

