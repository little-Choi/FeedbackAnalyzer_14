# 코드리뷰및개선보고서

## 1. 분석배경(프로젝트·목적)

**프로젝트명:** 리팩토링 챌린지 — 고객 피드백 분석 시스템 (Feedback Analyzer)

**기술 스택:** C++17, cpp-httplib, CMake, Catch2 v3

**목적:** 자연어 고객 피드백을 수집·분류·시각화하는 웹 애플리케이션의 레거시 코드에서 코드 스멜·안티패턴·기능 결함(P0/P1)을 식별하고, 테스트 Green을 유지하며 클린 아키텍처 방향으로 개선하는 실습 프로젝트이다.

**분석·개선 기준 문서**

| 문서 | 역할 |
|------|------|
| `project_purpose.md` | 학습 목표·의도적 스멜·실습 순서 |
| `docs/qa_analysis.md` | Cursor AI 코드 스멜·QA 통합 분석 |
| `docs/test_plan.md` | 테스트 ID·우선순위 |
| `docs/refactoring_plan.md` | Phase 0~7 리팩토링 계획 |
| `docs/refactoring_result.md` | Phase별 구현 결과 |

**분석 일자:** 2026-05-22  
**분석 대상:** `src/cpp/` 레거시 및 리팩토링 후 소스

---

## 2. Cursor AI 분석요약(항목별표)

### 2.1 QA·기능 결함

| 우선순위 | 문제 | 영향 | 처리 상태 |
|----------|------|------|-----------|
| P0 | 카테고리 필터가 `main` 키워드 무시 | 통계와 필터 결과 불일치 | 수정 완료 |
| P0 | 감성 분석·필터 이중 키워드 사전 | 화면 숫자 ≠ 필터 건수 | 수정 완료 |
| P0 | 중립 필터·통계 불일치 (`괜찮` 등) | 중립 선택 시 0건 | 수정 완료 |
| P1 | CSV `text` 컬럼 미사용 | README 명세 위반 | 수정 완료 |
| P1 | 다운로드·필터 결과 소스 분리 | 빈/불일치 다운로드 | 수정 완료 |
| P1 | CSV 업로드 후 통계 미표시 | 기능 누락 | 수정 완료 |
| P2 | 자동화 테스트 0건 | 회귀 검증 불가 | Catch2 74건 + Golden Master |
| P2 | 로그 UI level별 미표시 | 교육 과제 미완 | 미처리 |

### 2.2 코드 스멜·구조

| # | 스멜 | 주요 위치 | 위험도 | 처리 상태 |
|---|------|-----------|--------|-----------|
| 1 | 축약 메서드명 (`sent`, `kw`, `fil`) | TextAnalyzer, Filters | 중 | 신규 API + 레거시 alias |
| 2 | 중복 키워드·3중 카테고리 정의 | Constants, Filters, UIComponents | 높음 | 단일 사전·`categoryNames()` |
| 3 | Fake Session | Session.h | 높음 | FeedbackRepository facade |
| 4 | 전역 상태 (`globalSent`, `fil_data`) | main, TextAnalyzer | 높음 | 제거·Repository 통합 |
| 5 | FileHandler 스텁 | FileHandler.h | 중 | YAGNI 삭제 |
| 6 | God Module `main.cpp` | main.cpp | 중 | FeedbackApp·핸들러·Renderer 분리 |
| 7 | 감성·카테고리 로직 중복 | TextAnalyzer, Filters | 높음 | SentimentClassifier, CategoryMatcher |

### 2.3 테스트·회귀

| 항목 | 분석 시 | 개선 후 |
|------|---------|---------|
| 단위·통합 테스트 | 0건 | 74 tests Green |
| Golden Master | 없음 | `GM-*` + `tests/expected/` |
| CI | 없음 | `.github/workflows/tests.yml` |
| P0 회귀 태그 | — | `[p0]` ctest 필터 |

---

## 3. 개선/처리내역

| 피드백사항 | 수정조치 | 결과 |
|------------|----------|------|
| P0: 필터가 `main` 키워드 스킵 | `CategoryMatcher::matchesMain` + `CategoryRegistry`로 통계·필터 동일 규칙 | FI-F05, TA-K02 Green |
| P0: 감성 이중 사전 (`S_KEYWORDS`) | `initFilterKeywords` 제거, `SentimentClassifier` + `Constants::SENTIMENT_KEYWORDS` 단일화 | FI-F08, TA-S07/08 Green |
| P0: 중립·`괜찮` 불일치 | 감성 사전 통합; 부정에 `늦`, `늦다` 추가 | TA-S08, FI-F06 Green |
| P1: CSV 첫 컬럼만 사용 | `CsvHelpers` + `text` 헤더·컬럼 인식 | CSV-R02, GM-CSV Green |
| P1: `fil_data` vs Session 분리 | `FeedbackRepository::lastFiltered()` + `buildDownloadCsv` | CSV-R04, HTTP-H03 Green |
| P1: 업로드 후 통계 미표시 | Upload 핸들러에서 분석·렌더 연동 | HTTP-H04b Green |
| P2: 테스트 부재 | Catch2 64+ 케이스, Golden Master, CI workflow | 74/74 ctest Passed |
| 스멜: Constants 중복 블록 | 감성 키워드 중복 제거, 키 상수화 (`SENTIMENT_POSITIVE` 등) | CO-* Green |
| 스멜: UI `CATS` 3중 정의 | `Constants::categoryNames()` 단일 소스 | UI-01/02 Green |
| 스멜: God `main.cpp` | `FeedbackApp`, RouteHandler, `HtmlPageRenderer` 분리 | HTTP-*, GM-HTTP Green |
| 스멜: `containsAny` 중복 | `SentimentClassifier`, `CategoryMatcher` 추출 | TA-*, FI-* Green |
| 스멈: FileHandler 스텁 | 파일·테스트 삭제 (YAGNI) | FH 제거, 76→74 tests |
| 스멜: 매직 넘버 8080, BOM | `AppConfig.h` 상수화 | HTTP-*, CSV-* Green |
| API 네이밍 `sent`/`kw`/`fil` | `analyzeSentimentDistribution` 등 신규명 + alias | 전체 Green |
| CSV 콤마 이스케이프 | 계약 유지(미적용) — 의도적 현행 동작 | CSV-ESC Green |

---

## 4. 전후코드비교(cpp 코드블록)

### 4.1 카테고리 필터 — `main` 키워드 스킵 제거

**개선 전 (Filters — `main` 건너뜀):**

```cpp
for (const auto& subEntry : catMap) {
    if (subEntry.first == "main") continue;
    if (containsAny(txt, subEntry.second)) {
        finalFiltered.push_back(item);
    }
}
```

**개선 후 (Filters.h):**

```cpp
[[nodiscard]] static std::vector<Feedback> filterByCategory(
    const std::vector<Feedback>& dataList,
    const std::string& kFilter) {
    if (kFilter == AppConfig::FILTER_ALL) {
        return dataList;
    }
    std::vector<Feedback> filtered;
    for (const auto& item : dataList) {
        if (CategoryMatcher::matchesMain(item.getText(), kFilter)) {
            filtered.push_back(item);
        }
    }
    return filtered;
}
```

### 4.2 감성 분류 — 단일 사전·공유 Classifier

**개선 전 (TextAnalyzer 인라인 + Filters 별도 `S_KEYWORDS`):**

```cpp
if (containsAny(txt, Constants::SENTIMENT_KEYWORDS[u8"긍정"])) {
    s = u8"긍정";
} else if (containsAny(txt, Constants::SENTIMENT_KEYWORDS[u8"부정"])) {
    s = u8"부정";
}
// Filters: initFilterKeywords() → S_KEYWORDS (다른 목록)
```

**개선 후 (SentimentClassifier.h):**

```cpp
[[nodiscard]] static Sentiment classify(std::string_view text) {
    if (containsAny(text, Constants::SENTIMENT_KEYWORDS[Constants::SENTIMENT_POSITIVE])) {
        return Sentiment::Positive;
    }
    if (containsAny(text, Constants::SENTIMENT_KEYWORDS[Constants::SENTIMENT_NEGATIVE])) {
        return Sentiment::Negative;
    }
    return Sentiment::Neutral;
}
```

### 4.3 Constants — 중복·키 상수화

**개선 전:** 감성 키워드 블록 중복, `u8"긍정"` 등 매직 문자열 산재

**개선 후 (Constants.h / Constants.cpp):**

```cpp
static constexpr const char* SENTIMENT_POSITIVE = u8"긍정";
static constexpr const char* SENTIMENT_NEGATIVE = u8"부정";
static constexpr const char* SENTIMENT_NEUTRAL = u8"중립";
static constexpr const char* CATEGORY_MAIN_KEY = "main";

SENTIMENT_KEYWORDS[SENTIMENT_NEGATIVE] = {
    u8"나쁘", /* ... */ u8"늦", u8"늦다", u8"늦었", u8"늦어", /* ... */
};
```

### 4.4 main.cpp — God Module 분해

**개선 전:** HTTP 라우팅·HTML·CSV·전역 `fil_data`가 단일 파일에 혼재 (~350줄)

**개선 후 (main.cpp):**

```cpp
#include "FeedbackApp.h"

int main() {
    FeedbackApp app;
    return app.run();
}
```

라우트·렌더·저장은 `FeedbackApp.cpp`, `*RouteHandler`, `HtmlPageRenderer.cpp`, `FeedbackRepository.cpp`로 분리.

### 4.5 TextAnalyzer — 명확한 API·공유 Matcher

**개선 전:**

```cpp
std::map<std::string, int> sent(const std::vector<Feedback>& feedbacks);
std::map<std::string, int> kw(const std::vector<Feedback>& feedbacks);
// kw/fil 각각 containsAny 중복 구현
```

**개선 후 (TextAnalyzer.h):**

```cpp
[[nodiscard]] std::map<std::string, int> analyzeSentimentDistribution(
    const std::vector<Feedback>& feedbacks) {
    std::map<std::string, int> res;
    res[Constants::SENTIMENT_POSITIVE] = 0;
    res[Constants::SENTIMENT_NEUTRAL] = 0;
    res[Constants::SENTIMENT_NEGATIVE] = 0;
    for (const auto& f : feedbacks) {
        res[SentimentClassifier::classifyUtf8(f.getText())]++;
    }
    return res;
}
```

### 4.6 상태 관리 — `fil_data` → Repository

**개선 전:** `main.cpp` 전역 `fil_data`, `Session` 병행

**개선 후:** `FeedbackRepository::lastFiltered()` / `setLastFiltered()` 단일 저장소, `Session`은 thin facade

---

## 5. 효과및향후과제

### 5.1 효과

| 영역 | 효과 |
|------|------|
| **기능 정합성** | P0/P1 결함(품질 필터, 중립·`괜찮`, `늦었어요`, CSV `text`) 해소; 통계 수 = 필터 건수(FI-F08) |
| **유지보수성** | 키워드·감성·카테고리 규칙이 Constants + Classifier/Matcher 한 경로로 수렴 |
| **테스트 가능성** | 74 tests + Golden Master + CI로 리팩토링 단계별 회귀 방지 |
| **구조** | God `main.cpp` 축소, 책임별 모듈 분리(SRP), dead code·전역 상태 제거 |
| **가독성** | `analyzeSentimentDistribution` 등 의도 드러나는 API; `AppConfig`로 매직 넘버 제거 |

### 5.2 검증 결과

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
# 74/74 Passed (FileHandler YAGNI로 FH 2건 제거)
```

동작 계약(긍·부정 동시 → 긍정, main-only 카테고리, CSV `text`, BOM+헤더 download, 콤마 미이스케이프) 전항 Green.

### 5.3 향후 과제

| 우선순위 | 과제 | 비고 |
|----------|------|------|
| 교육 로드맵 | 로그 level별 UI 표시 (warning/error) | `project_purpose.md` §6.1 |
| 교육 로드맵 | 텍스트 입력 multi-line | UI 개선 |
| 기능 확장 | Trend 시각화 (`test_feedback_trend.csv`) | `docs/new_feature.md` |
| 기능 확장 | 감성 필터 File DB 관리 | 리팩토링 후 권장 |
| 기술 부채 | CSV 콤마 이스케이프 | `CSV-ESC` golden 갱신 필요 |
| 기술 부채 | 레거시 alias (`sent`/`kw`/`fil`) 제거 | 호출부 전면 이전 후 |
| 품질 | 커버리지 90% 목표 | `ENABLE_COVERAGE` 빌드 활용 |
| 운영 | Golden 의도 변경 시 `update_golden` 절차 준수 | `docs/golden_master.md` |

---

*작성: Cursor AI 분석·리팩토링 결과 종합 (2026-05-22)*
