# Feedback Analyzer — 리팩토링 계획

| 항목 | 내용 |
|------|------|
| **작성일** | 2026-05-22 |
| **기준 문서** | [README.md](../README.md), [qa_analysis.md](./qa_analysis.md), [project_purpose.md](../project_purpose.md) |
| **원칙** | **테스트 Green에서만** 다음 단계 진행 · **한 커밋 = 한 리팩토링 축** |
| **기준선** | 76 tests Green (`ctest --test-dir build`) |

---

## 1. 공통 게이트 (모든 단계)

```powershell
cd c:\DEV\FeedbackAnalyzer_14
cmake --build build
ctest --test-dir build --output-on-failure
```

| 선택 검증 | 명령 |
|-----------|------|
| P0 회귀 | `ctest --test-dir build -R "[p0]" --output-on-failure` |
| Golden Master | `ctest --test-dir build -R "GM-" --output-on-failure` |
| 도메인 집중 | `ctest --test-dir build -R "TA-\|FI-\|SE-\|CO-" --output-on-failure` |

**Golden 변경 시:** 의도된 출력 변경 후에만 `cmake --build build --target update_golden`

---

## 2. 현재 상태

### 완료 (P0/P1 + 테스트)

- 감성 사전 단일화 (`Constants::SENTIMENT_KEYWORDS`, `SentimentMatcher`)
- 카테고리 필터 `main` 키워드와 `TextAnalyzer::kw` 규칙 일치
- CSV `text` 컬럼·헤더 처리 (`CsvHelpers.h`)
- Catch2 76건 + Golden Master

### 남은 리팩토링 대상

| 영역 | 스멜 |
|------|------|
| 네이밍 | `sent`, `kw`, `fil` |
| 데이터 | `Constants` 중복 블록, `UIComponents::CATS` 3중 정의 |
| 상태 | `globalSent`/`globalKw`, `fil_data` vs `Session` |
| 구조 | God `main.cpp`, `FileHandler` 스텁, `Filters::fil` stdout |
| 상수 | `8080`, BOM 등 매직 넘버 |

---

## 3. 단계별 계획 (커밋 단위)

### Phase 0 — 기준선 고정

- [ ] `ctest` 100% Green 유지
- [ ] 동작 변경 없이 구조만 개선 (아래 §5 계약 준수)

---

### Phase 1 — 데이터·매직 넘버 (저위험, 3커밋)

| 커밋 | 목표 | 기법 | 검증 |
|------|------|------|------|
| **1-1** | `Constants.cpp` 감성 키워드 **중복 블록만** 제거; 키 문자열 상수화 | Remove Duplicates, Introduce Constant | `CO-*`, `[p0]` |
| **1-2** | `8080`, UTF-8 BOM, CSV 헤더 상수화 (`AppConfig` 또는 `Constants`) | Introduce Constant, C++17 `constexpr` | `HTTP-*`, `CSV-*`, `GM-CSV` |
| **1-3** | `categoryNames()` — `CATEGORY_KEYWORDS`에서 UI 카테고리 유도 | Single Source of Truth | `UI-*`, `CO-02` |

---

### Phase 2 — 조건 분기 축소·중복 제거 (중위험, 4커밋)

| 커밋 | 목표 | 기법 | 검증 |
|------|------|------|------|
| **2-1** | `SentimentClassifier` 클래스로 `classify`/`containsAny` 이동 (로직 동일) | Extract Class, Move Method | `TA-S*`, `FI-F*`, `GM-TA` |
| **2-2** | `Filters::fil` → 감성·카테고리 2단 파이프라인 (`전체` = no-op) | Extract Function, Guard Clauses | `FI-F*`, `GM-FI` (stdout 유지) |
| **2-3** | `CategoryMatcher` — `kw`와 `matchesCategoryMain` 단일 구현 | Extract Class, DRY | `TA-K*`, `FI-F05` |
| **2-4** | `initFilterKeywords()` 제거, `globalSent`/`globalKw` 미사용 시 제거 | Remove Dead Code | 전체 `ctest` |

---

### Phase 3 — 타입·정책 분리 (중~고위험, 3커밋)

| 커밋 | 목표 | 기법 | 검증 |
|------|------|------|------|
| **3-1** | `enum class Sentiment` + UTF-8 변환; S4 규칙(긍정 우선) 한 곳에 명시 | Replace Magic String, Policy object | `TA-S*`, `FI-F*`, `EX-*` |
| **3-2** | `CategoryRegistry` — 필터는 `main`만, 서브키는 통계 확장용 | Table-driven / Registry | P0 bundle, `FI-F05` |
| **3-3** | `sent`→`analyzeSentimentDistribution`, `kw`→`analyzeKeywordDistribution`, `fil`→`filterBySentimentAndCategory` | Rename Method | 전체 (+ alias 1커밋 선택) |

---

### Phase 4 — 상태·책임 분리 (고위험, 3커밋)

| 커밋 | 목표 | 기법 | 검증 |
|------|------|------|------|
| **4-1** | `Session` → `FeedbackRepository` (`add`, `all`, `lastFiltered`) | Replace Data Value with Object | `SE-*`, `HTTP-*` |
| **4-2** | `fil_data` 제거 → repository `lastFiltered` + `buildDownloadCsv` | Move Field | `CSV-R04`, `HTTP-H03`, `GM-CSV` |
| **4-3** | `Filters::fil` stdout — A: Logger 주입 유지 / B: 제거 + golden 갱신 | Introduce Observer 또는 Remove | `FI-P2-02`, `GM-FI` |

---

### Phase 5 — `main.cpp` 분해 (고위험, 3커밋)

| 커밋 | 목표 | 검증 |
|------|------|------|
| **5-1** | 라우트 핸들러 클래스 추출 (`Analyze`, `Filter`, `Upload`, `Download`) | `HTTP-*`, `EX-*`, `GM-HTTP` |
| **5-2** | `HtmlPageRenderer` + `PageViewModel` | `HTML-*`, `GM-HTTP` |
| **5-3** | `FeedbackApp` — static 서비스를 멤버로 | 전체 + 수동 스모크 |

---

### Phase 6 — FileHandler·CSV (2커밋)

| 커밋 | 선택 | 검증 |
|------|------|------|
| **6-1** | **A:** `FileHandler` 삭제(YAGNI) / **B:** `CsvFeedbackExporter` 실구현 | `FH-*` |
| **6-2** | CSV 이스케이프 (동작 변경 — `CSV-ESC` golden 갱신) | `CSV-*`, `GM-CSV` |

---

### Phase 7 — C++17 마무리 (2커밋)

| 커밋 | 내용 | 검증 |
|------|------|------|
| **7-1** | `std::string_view` 파라미터 (classify, CSV trim) | TA/FI/CSV |
| **7-2** | `[[nodiscard]]`, structured bindings, `std::optional` | 전체 + coverage 옵션 |

---

## 4. 진행 순서 (의존성)

```
Phase 0 (Green)
  → Phase 1 (Constants / Config / UI 단일화)
  → Phase 2 (Classifier / Filters 파이프라인 / CategoryMatcher)
  → Phase 3 (Enum / Registry / Rename)
  → Phase 4 (Repository / fil_data / stdout)
  → Phase 5 (main 분리)
  → Phase 6 (FileHandler / CSV escape)
  → Phase 7 (C++17 polish)
```

`project_purpose.md` §6.1 정렬: **네이밍·상수(1~3) → 긴 함수·중복(2,5) → FileHandler(6) → 추가 요구(Trend/DB)는 리팩토링 후**.

---

## 5. 동작 계약 (테스트가 고정 — 변경 금지)

1. 긍·부정 동시 포함 → **긍정** (TA-S04, if-else 순서).
2. 통계 감성 수 = 필터(동일 감성) 건수 (FI-F08).
3. 카테고리 필터 = **`main` 키워드만** (FI-F05, TA-K02).
4. CSV `id,text` → **text** 컬럼; 헤더 없는 1행 유지.
5. 필터 없이 download → BOM + `text` 헤더만 (CSV-R04).
6. CSV 콤마 미이스케이프 — `CSV-ESC` 유지 시 이스케이프는 별 PR.

---

## 6. 체크리스트 요약

| # | 커밋 | 완료 조건 | 필수 ctest |
|---|------|-----------|------------|
| 0 | Green | 76/76 | 전체 |
| 1-1 | Constants 중복 제거 | 키워드 집합 동일 | `CO-*`, `[p0]` |
| 1-2 | 포트/BOM 상수 | 동작 동일 | `HTTP-*`, `CSV-*`, `GM-CSV` |
| 1-3 | categoryNames | UI=Constants 5종 | `UI-*`, `CO-02` |
| 2-1 | SentimentClassifier | S4 규칙 유지 | `TA-S*`, `FI-F*` |
| 2-2 | fil 파이프라인 | AND 필터 동일 | `FI-F*`, `GM-FI` |
| 2-3 | CategoryMatcher | kw=fil 규칙 | `TA-K*`, `FI-F05` |
| 2-4 | dead code | 링크 OK | 전체 |
| 3-1 | Sentiment enum | 경계 변환만 | `TA-S*`, `EX-*` |
| 3-2 | CategoryRegistry | main 필터 | P0 bundle |
| 3-3 | rename API | 호출부 갱신 | 전체 |
| 4-1 | Repository | Session 대체 | `SE-*`, `HTTP-*` |
| 4-2 | fil_data 통합 | download 일치 | `CSV-R04`, `H03`, `GM-CSV` |
| 4-3 | stdout 정책 | golden 의도 확인 | `FI-P2-02`, `GM-FI` |
| 5-1~3 | main 분리 | God module 축소 | `HTTP-*`, `GM-HTTP`, `HTML-*` |
| 6 | FileHandler | 스텁 해소 | `FH-*`, `CSV-*` |
| 7 | C++17 | 경고 감소 | 전체 |

---

## 7. 관련 문서

- [qa_analysis.md](./qa_analysis.md) — 코드 스멜·P0/P1 분석·Phase 1~7 원안
- [test_plan.md](./test_plan.md) — 테스트 ID·우선순위
- [golden_master.md](./golden_master.md) — GM-* 갱신 절차
- [../project_purpose.md](../project_purpose.md) — 학습 목표·실습 순서

---

*코드 변경 시 본 문서의 완료 체크리스트와 §5 계약을 함께 갱신할 것.*
