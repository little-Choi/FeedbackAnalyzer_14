#include "HtmlPageRenderer.h"

#include "AppConfig.h"
#include "Constants.h"
#include "HtmlEscape.h"
#include "HttpFormUtils.h"
#include "UIComponents.h"

#include <sstream>

std::string HtmlPageRenderer::render(const PageViewModel& vm) {
    std::ostringstream html;
    html << R"(<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Feedback Analyzer</title>
    <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; }
        .container { max-width: 1200px; margin: 0 auto; background-color: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        h1 { color: #333; text-align: center; margin-bottom: 30px; }
        .section { margin-bottom: 30px; padding: 20px; border: 1px solid #ddd; border-radius: 5px; }
        .form-group { margin-bottom: 15px; }
        label { display: block; margin-bottom: 5px; font-weight: bold; color: #555; }
        input[type="text"], textarea, select { width: 100%; padding: 10px; border: 1px solid #ddd; border-radius: 4px; font-size: 14px; box-sizing: border-box; }
        textarea { height: 100px; resize: vertical; }
        button { background-color: #007bff; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; font-size: 14px; margin-right: 10px; }
        button:hover { background-color: #0056b3; }
        .btn-success { background-color: #28a745; }
        .btn-success:hover { background-color: #1e7e34; }
        .alert-success { background-color: #d4edda; border-color: #c3e6cb; color: #155724; padding: 10px; border-radius: 4px; }
        .alert-warning { background-color: #fff3cd; border-color: #ffeaa7; color: #856404; padding: 10px; border-radius: 4px; }
        .alert-danger { background-color: #f8d7da; border-color: #f5c6cb; color: #721c24; padding: 10px; border-radius: 4px; }
        .stats { display: flex; justify-content: space-around; margin: 20px 0; }
        .stat-item { text-align: center; padding: 15px; background-color: #f8f9fa; border-radius: 5px; flex: 1; margin: 0 10px; }
        .stat-number { font-size: 24px; font-weight: bold; color: #007bff; }
        .stat-label { color: #666; margin-top: 5px; }
        .trend-chart { display: flex; align-items: flex-end; gap: 12px; margin: 20px 0; min-height: 120px; }
        .trend-day { text-align: center; flex: 1; }
        .trend-date { font-size: 11px; color: #666; margin-bottom: 4px; }
        .trend-bar-wrap { display: flex; flex-direction: column-reverse; align-items: center; width: 36px; margin: 0 auto; }
        .trend-seg { width: 100%; min-height: 2px; }
        .trend-pos { background-color: #28a745; }
        .trend-neu { background-color: #6c757d; }
        .trend-neg { background-color: #dc3545; }
        .trend-total { font-size: 12px; color: #333; margin-top: 4px; }
        .trend-legend span { margin-right: 12px; font-size: 13px; }
        .trend-legend .dot { display: inline-block; width: 10px; height: 10px; margin-right: 4px; }
    </style>
</head>
<body>
<div class="container">
    <h1>Feedback Analyzer</h1>
    <p style="text-align: center; color: #666;">)" << u8"고객 피드백 분석 시스템" << R"(</p>)";

    if (!vm.success.empty()) {
        html << R"(<p class="alert alert-success">)" << getCurrentTimestamp() << " : "
             << escapeHtml(vm.success) << "</p>";
    }

    html << R"(
    <div class="section">
        <h3>)" << u8"피드백 입력" << R"(</h3>
        <form action="/analyze" method="post">
            <div class="form-group">
                <label for="text">)" << u8"피드백 텍스트:" << R"(</label>
                <textarea id="text" name="text" placeholder=")" << u8"피드백을 입력하세요..." << R"("></textarea>
            </div>
            <button type="submit">)" << u8"입력하기" << R"(</button>
        </form>
    </div>)";

    html << R"(
    <div class="section">
        <h3>)" << u8"CSV 파일 업로드 (선택사항)" << R"(</h3>
        <form action="/upload" method="post" enctype="multipart/form-data">
            <div class="form-group">
                <label for="file">)" << u8"CSV 파일 선택:" << R"(</label>
                <input type="file" id="file" name="file" accept=".csv">
            </div>
            <button type="submit">)" << u8"업로드" << R"(</button>
        </form>
        <form action="/trend/load-sample" method="post" style="margin-top:10px;">
            <button type="submit" class="btn-success">)" << u8"트렌드 샘플 CSV 로드" << R"(</button>
        </form>
    </div>)";

    const auto& cats = UIComponents::getCategories();
    html << R"(
    <div class="section">
        <h3>)" << u8"피드백 분석" << R"(</h3>
        <form action="/filter" method="post">
            <div class="form-group">
                <label for="sentiment">)" << u8"감정 필터:" << R"(</label>
                <select id="sentiment" name="sentiment">
                    <option value=")" << AppConfig::FILTER_ALL << R"(">)" << AppConfig::FILTER_ALL << R"(</option>
                    <option value=")" << Constants::SENTIMENT_POSITIVE << R"(">)" << Constants::SENTIMENT_POSITIVE << R"(</option>
                    <option value=")" << Constants::SENTIMENT_NEUTRAL << R"(">)" << Constants::SENTIMENT_NEUTRAL << R"(</option>
                    <option value=")" << Constants::SENTIMENT_NEGATIVE << R"(">)" << Constants::SENTIMENT_NEGATIVE << R"(</option>
                </select>
            </div>
            <div class="form-group">
                <label for="keyword">)" << u8"키워드 필터:" << R"(</label>
                <select id="keyword" name="keyword">
                    <option value=")" << AppConfig::FILTER_ALL << R"(">)" << AppConfig::FILTER_ALL << R"(</option>)";
    for (const auto& cat : cats) {
        html << R"(<option value=")" << cat << R"(">)" << cat << "</option>";
    }
    html << R"(
                </select>
            </div>
            <button type="submit">)" << u8"분  석" << R"(</button>
        </form>
    </div>)";

    if (!vm.warning.empty()) {
        html << R"(<p class="alert alert-warning">)" << escapeHtml(vm.warning) << "</p>";
    }

    if (!vm.sentimentResults.empty() || !vm.keywordResults.empty()) {
        html << R"(<div class="section"><h3>)" << u8"분석 결과" << "</h3>";
        if (!vm.sentimentResults.empty()) {
            html << "<h4>" << u8"감정 분포" << R"(</h4><div class="stats">)";
            for (const auto& entry : vm.sentimentResults) {
                html << R"(<div class="stat-item"><div class="stat-number">)" << entry.second
                     << R"(</div><div class="stat-label">)" << entry.first << "</div></div>";
            }
            html << "</div>";
        }
        if (!vm.keywordResults.empty()) {
            html << "<h4>" << u8"키워드 분포" << R"(</h4><div class="stats">)";
            for (const auto& entry : vm.keywordResults) {
                html << R"(<div class="stat-item"><div class="stat-number">)" << entry.second
                     << R"(</div><div class="stat-label">)" << entry.first << "</div></div>";
            }
            html << "</div>";
        }
        html << R"(<a href="/download"><button class="btn-success">)" << u8"결과 다운로드" << "</button></a></div>";
    }

    if (vm.showTrend && !vm.trendData.empty()) {
        html << R"(<div class="section"><h3>)" << u8"감정 트렌드 (날짜별)" << R"(</h3>)";
        html << R"(<div class="trend-legend"><span><span class="dot trend-pos"></span>)" << Constants::SENTIMENT_POSITIVE
             << R"(</span><span><span class="dot trend-neu"></span>)" << Constants::SENTIMENT_NEUTRAL
             << R"(</span><span><span class="dot trend-neg"></span>)" << Constants::SENTIMENT_NEGATIVE
             << R"(</span></div>)";
        html << TrendAnalyzer::renderTrendChartHtml(vm.trendData);
        html << "</div>";
    }

    if (!vm.sentimentKeywords.empty()) {
        html << R"(<div class="section"><h3>)" << u8"감정 키워드 DB 관리" << R"(</h3>)";
        if (!vm.adminMessage.empty()) {
            html << R"(<p class="alert alert-success">)" << escapeHtml(vm.adminMessage) << "</p>";
        }
        for (const auto& entry : vm.sentimentKeywords) {
            html << "<h4>" << entry.first << "</h4><ul>";
            for (const auto& kw : entry.second) {
                html << "<li>" << escapeHtml(kw)
                     << R"( <form action="/admin/sentiment" method="post" style="display:inline;">)"
                     << R"(<input type="hidden" name="action" value="remove">)"
                     << R"(<input type="hidden" name="sentiment" value=")" << entry.first << R"(">)"
                     << R"(<input type="hidden" name="keyword" value=")" << escapeHtml(kw) << R"(">)"
                     << R"(<button type="submit">)" << u8"삭제" << "</button></form></li>";
            }
            html << "</ul>";
        }
        html << R"(
        <form action="/admin/sentiment" method="post">
            <input type="hidden" name="action" value="add">
            <div class="form-group">
                <label>)" << u8"감정:" << R"(</label>
                <select name="sentiment">
                    <option value=")" << Constants::SENTIMENT_POSITIVE << R"(">)" << Constants::SENTIMENT_POSITIVE << R"(</option>
                    <option value=")" << Constants::SENTIMENT_NEGATIVE << R"(">)" << Constants::SENTIMENT_NEGATIVE << R"(</option>
                    <option value=")" << Constants::SENTIMENT_NEUTRAL << R"(">)" << Constants::SENTIMENT_NEUTRAL << R"(</option>
                </select>
            </div>
            <div class="form-group">
                <label>)" << u8"키워드:" << R"(</label>
                <input type="text" name="keyword" required>
            </div>
            <button type="submit">)" << u8"키워드 추가" << R"(</button>
        </form></div>)";
    }

    if (!vm.error.empty()) {
        html << R"(<p class="alert alert-danger">)" << escapeHtml(vm.error) << "</p>";
    }

    html << "</div></body></html>";
    return html.str();
}
