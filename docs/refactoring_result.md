# Feedback Analyzer — 리팩토링 결과

| 항목 | 내용 |
|------|------|
| **작성일** | 2026-05-22 |
| **기준** | [refactoring_plan.md](./refactoring_plan.md) |
| **원칙** | 테스트 Green에서만 단계 진행 |
| **최종 검증** | `ctest --test-dir build` → **74/74 Passed** |

---

## Phase 0 — 기준선 고정

| 항목 | 결과 |
|------|------|
| 시작 | 76 tests Green |
| 완료 | 76/76 Passed (리팩토링 전) |
| 동작 | 변경 없음 |

---

## Phase 1 — 데이터·매직 넘버

### 1-1 Constants 중복 제거·키 상수화

| 변경 | 파일 |
|------|------|
| 감성 키워드 중복 블록 제거 | `Constants.cpp` |
| `SENTIMENT_POSITIVE` / `NEGATIVE` / `NEUTRAL`, `CATEGORY_MAIN_KEY` | `Constants.h` |
| CO-04 호환: `좋아요` 1건 중복 유지 | `Constants.cpp` |

**검증:** `CO-*`, `[p0]` — Passed

### 1-2 AppConfig 상수화

| 상수 | 값 |
|------|-----|
| `HTTP_PORT` | 8080 |
| `UTF8_BOM` | `\xEF\xBB\xBF` |
| `CSV_TEXT_HEADER` | `text` |
| `FILTER_ALL` | `전체` |

**파일:** `AppConfig.h`, `CsvHelpers.h`, `FeedbackApp.cpp`

**검증:** `HTTP-*`, `CSV-*`, `GM-CSV` — Passed

### 1-3 categoryNames() 단일화

| 변경 | 설명 |
|------|------|
| `Constants::categoryNames()` | UI 표시 순서(배송→품질→가격→서비스→사용성) |
| `UIComponents::getCategories()` | `categoryNames()` 위임, `CATS` 제거 |

**검증:** `UI-*`, `CO-02` — Passed

---

## Phase 2 — 조건 분기·중복 제거

### 2-1 SentimentClassifier

| 클래스 | 책임 |
|--------|------|
| `SentimentClassifier` | `containsAny`, `classify` (S4: 긍정 우선) |
| `SentimentMatcher.h` | 레거시 `classifySentiment()` 래퍼 |

**검증:** `TA-S*`, `FI-F*`, `GM-TA` — Passed

### 2-2 Filters 파이프라인

| 단계 | 메서드 |
|------|--------|
| 1 | `filterBySentiment` (`전체` = no-op) |
| 2 | `filterByCategory` |
| 3 | `printFilteredToStdout` (stdout 유지) |

**검증:** `FI-F*`, `GM-FI` — Passed

### 2-3 CategoryMatcher

| 클래스 | 책임 |
|--------|------|
| `CategoryMatcher` | `matchesMain`, `containsAny` |
| `CategoryRegistry` | `main` 키워드 조회 |

**검증:** `TA-K*`, `FI-F05` — Passed

### 2-4 Dead code 제거

| 제거 | 비고 |
|------|------|
| `initFilterKeywords()` | 호출부 전부 삭제 |
| `globalSent` / `globalKw` | `TextAnalyzer`에서 제거 |

**검증:** 전체 ctest — Passed

---

## Phase 3 — 타입·정책·네이밍

### 3-1 enum class Sentiment

| 파일 | 내용 |
|------|------|
| `Sentiment.h` | `Positive` / `Negative` / `Neutral` |
| | `sentimentToUtf8`, `sentimentFromUtf8` |

**검증:** `TA-S*`, `FI-F*`, `EX-*` — Passed

### 3-2 CategoryRegistry

- 필터·통계는 `main` 키워드만 (`CategoryRegistry::mainKeywords`)

**검증:** P0 bundle, `FI-F05` — Passed

### 3-3 API Rename

| 레거시 | 신규 |
|--------|------|
| `sent()` | `analyzeSentimentDistribution()` |
| `kw()` | `analyzeKeywordDistribution()` |
| `fil()` | `filterBySentimentAndCategory()` |

레거시 alias는 테스트·HttpFixture 호환을 위해 유지.

**검증:** 전체 — Passed

---

## Phase 4 — 상태·책임 분리

### 4-1 FeedbackRepository

| API | 역할 |
|-----|------|
| `all()` | 전체 피드백 |
| `lastFiltered()` | 마지막 필터 결과 |
| `setLastFiltered()` | 필터 후 저장 |

`Session`은 `FeedbackRepository` thin facade.

**검증:** `SE-*`, `HTTP-*` — Passed

### 4-2 fil_data 제거

| 이전 | 이후 |
|------|------|
| `main.cpp` `fil_data` | `FeedbackRepository::lastFiltered()` |
| download 인라인 | `buildDownloadCsv(lastFiltered())` |

**검증:** `CSV-R04`, `HTTP-H03`, `GM-CSV` — Passed

### 4-3 stdout 정책

- **선택 A:** `Filters::printFilteredToStdout` 유지 (golden·FI-P2-02 동일)

**검증:** `FI-P2-02`, `GM-FI` — Passed

---

## Phase 5 — main.cpp 분해

| 파일 | 책임 |
|------|------|
| `main.cpp` | `FeedbackApp::run()` 진입만 |
| `FeedbackApp.cpp` | 라우트 등록·핸들러 조립 |
| `AnalyzeRouteHandler` | POST `/analyze` |
| `UploadRouteHandler` | POST `/upload` |
| `FilterRouteHandler` | POST `/filter` |
| `DownloadRouteHandler` | GET `/download` |
| `HtmlPageRenderer.cpp` | `PageViewModel` → HTML |
| `HtmlEscape.h` | HTML 이스케이프 |
| `HttpFormUtils.h` | `parseForm`, `urlDecode` |

**검증:** `HTTP-*`, `EX-*`, `GM-HTTP`, `HTML-*` — Passed

---

## Phase 6 — FileHandler·CSV

### 6-1 FileHandler 삭제 (YAGNI)

| 조치 | |
|------|--|
| `FileHandler.h` 삭제 | |
| `test_file_handler.cpp` 삭제 | FH-01/02 제거 |
| `main` / 테스트에서 참조 제거 | |

**테스트 수:** 76 → **74** (FH 2건 제거)

### 6-2 CSV 이스케이프

- **미적용** — §5 계약·`CSV-ESC` 유지 (콤마 미이스케이프 현행)

**검증:** `CSV-*`, `GM-CSV` — Passed

---

## Phase 7 — C++17 마무리

| 항목 | 적용 |
|------|------|
| `std::string_view` | `SentimentClassifier`, `CategoryMatcher`, `HtmlEscape`, `trimCsvField` |
| `[[nodiscard]]` | 분석·필터·escape 반환값 |
| `std::optional` | — (필요 최소 범위로 미도입) |

**검증:** 전체 74 tests — Passed

---

## 최종 소스 구조

```
src/cpp/
├── main.cpp                 # 진입점
├── FeedbackApp.cpp/h        # HTTP 서버·라우트
├── FeedbackRepository.cpp/h # 피드백·필터 결과 저장
├── Session.h                # Repository facade
├── Constants.cpp/h
├── AppConfig.h
├── Sentiment.h
├── SentimentClassifier.h
├── CategoryRegistry.h
├── CategoryMatcher.h
├── SentimentMatcher.h       # 레거시 inline 래퍼
├── TextAnalyzer.h
├── Filters.h
├── CsvHelpers.h
├── HtmlPageRenderer.cpp/h
├── HtmlEscape.h
├── HttpFormUtils.h
├── PageViewModel.h
├── UIComponents.cpp/h
├── Logger.cpp/h
├── Feedback.h
└── httplib.h
```

**삭제:** `FileHandler.h`

---

## 동작 계약 (§5) 준수 확인

| # | 계약 | 상태 |
|---|------|------|
| 1 | 긍·부정 동시 → 긍정 | Green (`TA-S04`) |
| 2 | sent(긍정) = fil(긍정).size | Green (`FI-F08`) |
| 3 | 카테고리 필터 = main만 | Green (`FI-F05`, `TA-K02`) |
| 4 | CSV id,text → text 컬럼 | Green (`CSV-R02`) |
| 5 | 필터 없이 download → BOM+헤더만 | Green (`CSV-R04`) |
| 6 | CSV 콤마 미이스케이프 | Green (`CSV-ESC`) |

---

## 검증 명령

```powershell
cd c:\DEV\FeedbackAnalyzer_14
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R "[p0]" --output-on-failure
ctest --test-dir build -R "GM-" --output-on-failure
```

---

## 요약

- **Phase 0~7** 계획 항목을 코드에 반영했고, 각 단계 후 **ctest Green**을 유지했다.
- **74 tests** 전부 통과 (FileHandler YAGNI로 FH 2건 제거).
- God `main.cpp` (~350줄) → **진입점 + 모듈 분리**로 축소.
- 상태는 `FeedbackRepository` 단일 저장소로 통합; `fil_data` / `globalSent` / `globalKw` / `initFilterKeywords` 제거.

*의도된 동작 변경 시 Golden Master는 `cmake --build build --target update_golden` 후 갱신할 것.*
