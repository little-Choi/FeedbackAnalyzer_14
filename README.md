# Feedback Analyzer
![feedback_analyzer](./feedback_analyzer.png)

고객 피드백 분석 시스템은 자연어 기반 고객 피드백 데이터를 수집, 분류, 시각화하는 기능을 제공하는 C++ (cpp-httplib) 기반 웹 애플리케이션입니다.

## 주요 기능

- 텍스트 피드백 입력 (수동/CSV 업로드)
- 키워드 기반 피드백 분류
- 감정 분석 (긍정/부정/중립)
- 피드백 필터링 및 검색
- 분석 결과 시각화
- 결과 CSV 다운로드

## 요구사항

- C++17 이상 지원 컴파일러 (MSVC, GCC, Clang)
- CMake 3.14 이상

## 설치 방법
저장소 클론
```
git clone [repository-url]
cd feedback_analyzer_cpp
```

## 빌드 방법
```
rmdir /q /s build
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
cmake --build build
```

## 실행 방법
```
build\feedback_analyzer.exe
```

## 프로젝트 구조

```
feedback_analyzer_cpp/
├── src/cpp/
│   ├── main.cpp           # HTTP 서버 및 라우팅 (cpp-httplib 기반)
│   ├── httplib.h           # cpp-httplib 헤더 라이브러리
│   ├── Feedback.h          # 피드백 데이터 모델
│   ├── TextAnalyzer.h/cpp  # 텍스트 분석 로직
│   ├── Filters.h/cpp       # 필터링
│   ├── UIComponents.h/cpp  # UI 컴포넌트
│   ├── Session.h/cpp       # 상태 관리
│   ├── Logger.h/cpp        # 로깅
│   ├── Constants.h/cpp     # 상수 정의
│   └── FileHandler.h       # 파일 처리
├── tests/                  # Catch2 단위·통합 테스트
│   ├── unit/               # 도메인·성능·Logger·UI·FileHandler
│   └── integration/        # CSV, HTTP, 예외
├── .github/workflows/      # regression / known-fail / coverage CI
├── docs/test_plan.md       # 테스트 계획서
├── CMakeLists.txt          # CMake 빌드 설정
├── project_purpose.md      # 프로젝트 목적 문서
└── README.md               # 프로젝트 설명
```

## 사용 방법

1. 웹 브라우저에서 `http://localhost:8080` 접속
2. 피드백 텍스트 입력 또는 CSV 파일 업로드
3. 감정/키워드 필터로 결과 필터링
4. 필요시 결과 다운로드

## CSV 파일 형식

입력 CSV 파일은 다음과 같은 형식이어야 합니다:
- 필수 컬럼: `text`
- 텍스트 컬럼에 피드백 내용 포함

## 테스트

기준: [docs/test_plan.md](docs/test_plan.md) · [docs/qa_analysis.md](docs/qa_analysis.md) · C++17 · Catch2 v3

### 빌드 및 실행

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
cmake --build build
ctest --test-dir build --output-on-failure
# 또는
build/feedback_analyzer_tests.exe
```

태그 예: `feedback_analyzer_tests "[p0]"` · Golden Master: `[golden]` · 알려진 결함: `[known-fail]`

**Golden Master (출력 회귀):** [docs/golden_master.md](docs/golden_master.md) · 기대값 `tests/expected/`

```bash
ctest --test-dir build -R "GM-" --output-on-failure
# baseline 갱신 (의도된 동작 변경 후)
cmake --build build --target update_golden
# 또는 .\scripts\update_golden.ps1
```

```bash
# 커버리지 빌드 (MinGW gcov)
cmake -S . -B build-cov -DENABLE_COVERAGE=ON -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
cmake --build build-cov --target feedback_analyzer_coverage
build-cov/tests/feedback_analyzer_coverage.exe

# CI와 동일: known-fail 태그만 (Catch2 필터)
build/tests/feedback_analyzer_tests.exe "[known-fail]"
build/tests/feedback_analyzer_tests.exe "~[known-fail]"
```

### 테스트 케이스 (Green — 64 cases / 135 assertions)

| 상태 | ID | 모듈 | 설명 | 소스 |
|------|-----|------|------|------|
| [x] | FB-01 | Feedback | 생성·텍스트 조회 | `tests/unit/test_feedback.cpp` |
| [x] | FB-02 | Feedback | 빈 문자열 저장 | `tests/unit/test_feedback.cpp` |
| [x] | FB-03 | Feedback | UTF-8 한글 보존 | `tests/unit/test_feedback.cpp` |
| [x] | FB-04 | Feedback | 장문(10KB+) 저장 | `tests/unit/test_feedback.cpp` |
| [x] | TA-S01 | TextAnalyzer::sent | 긍정 단일 | `tests/unit/test_text_analyzer.cpp` |
| [x] | TA-S02 | TextAnalyzer::sent | 부정 | `tests/unit/test_text_analyzer.cpp` |
| [x] | TA-S03 | TextAnalyzer::sent | 키워드 없음 → 중립 | `tests/unit/test_text_analyzer.cpp` |
| [x] | TA-S04 | TextAnalyzer::sent | 긍·부정 혼재 → 긍정 (if-else 순서) | `tests/unit/test_text_analyzer.cpp` |
| [x] | TA-S05 | TextAnalyzer::sent | 빈 목록 | `tests/unit/test_text_analyzer.cpp` |
| [x] | TA-S06 | TextAnalyzer::sent | 분포 합계 = N | `tests/unit/test_text_analyzer.cpp` |
| [x] | TA-S07 | TextAnalyzer::sent | `배송이 늦었어요` → 부정 | `tests/unit/test_text_analyzer.cpp` |
| [x] | TA-S08 | TextAnalyzer::sent | `괜찮아요` → 중립 | `tests/unit/test_text_analyzer.cpp` |
| [x] | TA-K01 | TextAnalyzer::kw | main 키워드·배송 | `tests/unit/test_text_analyzer.cpp` |
| [x] | TA-K02 | TextAnalyzer::kw | `품질이 좋습니다` | `tests/unit/test_text_analyzer.cpp` |
| [x] | TA-K03 | TextAnalyzer::kw | 복수 카테고리 | `tests/unit/test_text_analyzer.cpp` |
| [x] | TA-K04 | TextAnalyzer::kw | 미매칭 | `tests/unit/test_text_analyzer.cpp` |
| [x] | TA-K05 | TextAnalyzer::kw | `배송지연` | `tests/unit/test_text_analyzer.cpp` |
| [x] | FI-F01 | Filters::fil | 긍정+전체 | `tests/unit/test_filters.cpp` |
| [x] | FI-F02 | Filters::fil | 전체+배송 | `tests/unit/test_filters.cpp` |
| [x] | FI-F03 | Filters::fil | 긍정+배송 AND | `tests/unit/test_filters.cpp` |
| [x] | FI-F04 | Filters::fil | 빈 데이터 | `tests/unit/test_filters.cpp` |
| [x] | FI-F05 | Filters::fil | 품질+전체 (main 키워드) | `tests/unit/test_filters.cpp` |
| [x] | FI-F06 | Filters::fil | 중립+전체 (`괜찮아요`) | `tests/unit/test_filters.cpp` |
| [x] | FI-F07 | Filters::fil | 부정+`늦다` | `tests/unit/test_filters.cpp` |
| [x] | FI-F07b | Filters::fil | 부정+`늦었어요` | `tests/unit/test_filters.cpp` |
| [x] | FI-F08 | Filters::fil | sent(긍정) = fil(긍정).size | `tests/unit/test_filters.cpp` |
| [x] | FI-P2-02 | Filters::fil | stdout 부작용 | `tests/unit/test_filters.cpp` |
| [x] | SE-R01 | Session | update 후 size | `tests/unit/test_session.cpp` |
| [x] | SE-R02 | Session | key 무시 | `tests/unit/test_session.cpp` |
| [x] | SE-R03 | Session | 반환 참조 수정 반영 | `tests/unit/test_session.cpp` |
| [x] | SE-R04 | Session | static 격리 | `tests/unit/test_session.cpp` |
| [x] | CO-01 | Constants | init idempotent | `tests/unit/test_constants.cpp` |
| [x] | CO-02 | Constants | 카테고리 5종 | `tests/unit/test_constants.cpp` |
| [x] | CO-03 | Constants | `main` 키 존재 | `tests/unit/test_constants.cpp` |
| [x] | CO-04 | Constants | 긍정 키워드 중복 허용 | `tests/unit/test_constants.cpp` |
| [x] | CSV-R02 | CSV | `id,text` → `text` 컬럼 | `tests/integration/test_csv_behavior.cpp` |
| [x] | CSV-R04 | CSV | 필터 없이 download → 헤더만 | `tests/integration/test_csv_behavior.cpp` |
| [x] | CSV-ESC | CSV | 콤마 미이스케이프(현행) | `tests/integration/test_csv_behavior.cpp` |
| [x] | CSV-NH01 | CSV | 헤더 없음 1행 유지 | `tests/integration/test_csv_behavior.cpp` |
| [x] | HTTP-H01 | HTTP | POST `/analyze` | `tests/integration/test_http_routes.cpp` |
| [x] | HTTP-H02 | HTTP | POST `/filter` 데이터 없음 | `tests/integration/test_http_routes.cpp` |
| [x] | HTTP-H03 | HTTP | filter 후 `/download` | `tests/integration/test_http_routes.cpp` |
| [x] | HTTP-H04 | HTTP | POST `/upload` CSV | `tests/integration/test_http_routes.cpp` |
| [x] | HTTP-H04b | HTTP | 업로드 후 통계 표시 | `tests/integration/test_http_exceptions.cpp` |
| [x] | EX-01~05 | HTTP | 예외·invalid sentiment | `tests/integration/test_http_exceptions.cpp` |
| [x] | PERF-01 | 성능 | 1000+·10s 이내 | `tests/unit/test_performance.cpp` |
| [x] | LG-01~05 | Logger | log smoke·debug | `tests/unit/test_logger.cpp` |
| [x] | UI-01~02 | UIComponents | CAT 5종 | `tests/unit/test_ui_components.cpp` |
| [x] | FH-01~02 | FileHandler | 스텁 stdout | `tests/unit/test_file_handler.cpp` |
| [x] | HTML-01 | HTML | `escapeHtml` 특수문자 | `tests/unit/test_html_escape.cpp` |
| [x] | HTML-02 | HTML | apostrophe `&#39;` | `tests/unit/test_html_escape.cpp` |
| [x] | coverage | gcov | `-DENABLE_COVERAGE=ON` | `tests/CMakeLists.txt` |
| [x] | CI | GitHub Actions | regression + `~[known-fail]` | `.github/workflows/tests.yml` |

### 레거시 수정 요약 (2026-05-22)

[docs/qa_analysis.md](docs/qa_analysis.md) P0/P1 결함을 `src/cpp/`에서 수정했습니다. **64 test cases · 135 assertions 전부 Green.**

| 영역 | 변경 |
|------|------|
| **Filters** | 카테고리 필터가 `TextAnalyzer::kw`와 동일하게 `main` 키워드 사용; 감성 필터가 `Constants::SENTIMENT_KEYWORDS` 단일 사전 사용 (`S_KEYWORDS` 제거) |
| **Constants** | 부정 키워드에 `늦`, `늦다` 추가 → `늦었어요` 등과 통계·필터 일치 |
| **main.cpp** | CSV `text` 컬럼 인식·헤더 없는 1행 유지; 업로드 후 `sent`/`kw` 통계 렌더; `escapeHtml`에 `'` → `&#39;` |
| **테스트** | `[known-fail]` 기대값을 수정된 동작에 맞게 갱신 (`LegacyCsv.h`, `LegacyHtml.h`, `HttpFixture.h` 동기화) |

남은 개선(리팩토링 범위): `fil_data` vs Session 분리, CSV 다운로드 이스케이프, `FileHandler` 실구현, HTML/라우트 분리 — [test_plan.md §8](docs/test_plan.md)
