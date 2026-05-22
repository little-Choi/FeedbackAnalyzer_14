#include "FeedbackApp.h"

#include "AppConfig.h"
#include "Constants.h"
#include "CsvHelpers.h"
#include "FeedbackRepository.h"
#include "Filters.h"
#include "HtmlPageRenderer.h"
#include "HttpFormUtils.h"
#include "Logger.h"
#include "PageViewModel.h"
#include "Session.h"
#include "TextAnalyzer.h"
#include "httplib.h"

class AnalyzeRouteHandler {
public:
    explicit AnalyzeRouteHandler(TextAnalyzer& analyzer) : analyzer_(analyzer) {}

    void handle(const httplib::Request& req, httplib::Response& res) const {
        try {
            auto& feedbacks = FeedbackRepository::all();
            auto params = parseForm(req.body);
            std::string text = params["text"];

            if (!text.empty()) {
                const auto start = text.find_first_not_of(" \t\r\n");
                const auto end = text.find_last_not_of(" \t\r\n");
                if (start != std::string::npos) {
                    text = text.substr(start, end - start + 1);
                    feedbacks.emplace_back(text);
                }
            }

            for (const auto& fb : feedbacks) {
                Logger::logInfo(fb.getText());
            }
            Logger::logInfo(u8"현재 " + std::to_string(feedbacks.size()) + u8"개의 피드백이 입력되었습니다.");

            PageViewModel vm;
            vm.success = std::to_string(feedbacks.size()) + u8"개의 피드백이 입력되었습니다.";
            vm.feedbacks = feedbacks;
            if (!feedbacks.empty()) {
                vm.sentimentResults = analyzer_.analyzeSentimentDistribution(feedbacks);
                vm.keywordResults = analyzer_.analyzeKeywordDistribution(feedbacks);
                Logger::logInfo(u8"감성 분석 완료");
                Logger::logInfo(u8"키워드 분석 완료");
            }
            res.set_content(HtmlPageRenderer::render(vm), "text/html; charset=UTF-8");
        } catch (const std::exception& e) {
            Logger::logError(std::string(u8"오류 발생: ") + e.what());
            PageViewModel vm;
            vm.error = u8"처리 중 오류가 발생했습니다.";
            res.set_content(HtmlPageRenderer::render(vm), "text/html; charset=UTF-8");
        }
    }

private:
    TextAnalyzer& analyzer_;
};

class UploadRouteHandler {
public:
    explicit UploadRouteHandler(TextAnalyzer& analyzer) : analyzer_(analyzer) {}

    void handle(const httplib::Request& req, httplib::Response& res) const {
        try {
            auto& feedbacks = FeedbackRepository::all();
            if (req.form.has_file("file")) {
                const auto file = req.form.get_file("file");
                if (!file.content.empty()) {
                    appendFeedbackFromCsvContent(file.content, feedbacks);
                    Logger::logInfo(u8"파일이 성공적으로 업로드되었습니다.");
                }
            }
            PageViewModel vm;
            vm.success = std::to_string(feedbacks.size()) + u8"개의 피드백이 입력되었습니다.";
            vm.feedbacks = feedbacks;
            if (!feedbacks.empty()) {
                vm.sentimentResults = analyzer_.analyzeSentimentDistribution(feedbacks);
                vm.keywordResults = analyzer_.analyzeKeywordDistribution(feedbacks);
            }
            res.set_content(HtmlPageRenderer::render(vm), "text/html; charset=UTF-8");
        } catch (const std::exception& e) {
            Logger::logError(std::string(u8"파일 업로드 오류: ") + e.what());
            PageViewModel vm;
            vm.error = u8"파일 업로드 중 오류가 발생했습니다.";
            res.set_content(HtmlPageRenderer::render(vm), "text/html; charset=UTF-8");
        }
    }

private:
    TextAnalyzer& analyzer_;
};

class FilterRouteHandler {
public:
    FilterRouteHandler(TextAnalyzer& analyzer, Filters& filters)
        : analyzer_(analyzer), filters_(filters) {}

    void handle(const httplib::Request& req, httplib::Response& res) const {
        try {
            auto& feedbacks = FeedbackRepository::all();
            auto params = parseForm(req.body);
            const std::string sentiment = params["sentiment"];
            const std::string keyword = params["keyword"];

            if (feedbacks.empty()) {
                Logger::logWarning(u8"분석할 피드백이 없습니다.");
                PageViewModel vm;
                vm.warning = u8"분석할 피드백이 없습니다.";
                res.set_content(HtmlPageRenderer::render(vm), "text/html; charset=UTF-8");
                return;
            }

            auto filtered = filters_.filterBySentimentAndCategory(feedbacks, sentiment, keyword);
            if (filtered.empty()) {
                Logger::logWarning(u8"필터링 결과가 없습니다.");
                PageViewModel vm;
                vm.warning = u8"필터링 결과가 없습니다.";
                res.set_content(HtmlPageRenderer::render(vm), "text/html; charset=UTF-8");
                return;
            }

            FeedbackRepository::setLastFiltered(filtered);
            Logger::logInfo(u8"필터링 결과: " + std::to_string(filtered.size()) + u8"개의 피드백");
            PageViewModel vm;
            vm.sentimentResults = analyzer_.analyzeSentimentDistribution(filtered);
            vm.keywordResults = analyzer_.analyzeKeywordDistribution(filtered);
            vm.feedbacks = filtered;
            res.set_content(HtmlPageRenderer::render(vm), "text/html; charset=UTF-8");
        } catch (const std::exception& e) {
            Logger::logError(std::string(u8"오류 발생: ") + e.what());
            PageViewModel vm;
            vm.error = u8"처리 중 오류가 발생했습니다.";
            res.set_content(HtmlPageRenderer::render(vm), "text/html; charset=UTF-8");
        }
    }

private:
    TextAnalyzer& analyzer_;
    Filters& filters_;
};

class DownloadRouteHandler {
public:
    void handle(const httplib::Request&, httplib::Response& res) const {
        const std::string csv = buildDownloadCsv(FeedbackRepository::lastFiltered());
        res.set_header("Content-Disposition", "attachment; filename=\"filtered_feedback.csv\"");
        res.set_content(csv, "text/csv; charset=UTF-8");
    }
};

int FeedbackApp::run() {
    Constants::init();
    FeedbackRepository::init();
    Session::initSessionStateUgly();

    httplib::Server svr;
    TextAnalyzer textAnalyzer;
    Filters filters;

    AnalyzeRouteHandler analyzeHandler(textAnalyzer);
    UploadRouteHandler uploadHandler(textAnalyzer);
    FilterRouteHandler filterHandler(textAnalyzer, filters);
    DownloadRouteHandler downloadHandler;

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        Session::initSessionStateUgly();
        PageViewModel vm;
        vm.success = u8"피드백 분석기 시작";
        vm.feedbacks = FeedbackRepository::all();
        res.set_content(HtmlPageRenderer::render(vm), "text/html; charset=UTF-8");
    });

    svr.Post("/analyze", [&analyzeHandler](const httplib::Request& req, httplib::Response& res) {
        analyzeHandler.handle(req, res);
    });

    svr.Post("/upload", [&uploadHandler](const httplib::Request& req, httplib::Response& res) {
        uploadHandler.handle(req, res);
    });

    svr.Post("/filter", [&filterHandler](const httplib::Request& req, httplib::Response& res) {
        filterHandler.handle(req, res);
    });

    svr.Get("/download", [&downloadHandler](const httplib::Request& req, httplib::Response& res) {
        downloadHandler.handle(req, res);
    });

    Logger::logInfo(u8"서버가 http://localhost:" + std::to_string(AppConfig::HTTP_PORT) +
                    u8" 에서 시작됩니다.");
    svr.listen("0.0.0.0", AppConfig::HTTP_PORT);
    return 0;
}
