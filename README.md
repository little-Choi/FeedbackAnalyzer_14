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

기준: [docs/test_plan.md](docs/test_plan.md) · C++17 · Catch2 v3 · 레거시 `src/cpp/` 비수정

### 빌드 및 실행

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
cmake --build build
ctest --test-dir build --output-on-failure
# 또는
build/feedback_analyzer_tests.exe
```

태그 예: `feedback_analyzer_tests "[p0]"` · 알려진 결함: `[known-fail]`

```bash
# 커버리지 빌드 (MinGW gcov)
cmake -S . -B build-cov -DENABLE_COVERAGE=ON -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
cmake --build build-cov --target feedback_analyzer_coverage
build-cov/tests/feedback_analyzer_coverage.exe

# CI와 동일: known-fail 태그만 (Catch2 필터)
build/tests/feedback_analyzer_tests.exe "[known-fail]"
build/tests/feedback_analyzer_tests.exe "~[known-fail]"
```

### Test To Do list

| 상태 | ID | 모듈 | 설명 | 태그 |
|------|-----|------|------|------|
| [x] | FB-01~04 | Feedback | 생성·빈문자열·UTF-8·장문 | `tests/unit/test_feedback.cpp` |
| [x] | TA-S01~08 | TextAnalyzer::sent | 감정 분포 P0/P1 | `test_text_analyzer.cpp` |
| [x] | TA-K01~05 | TextAnalyzer::kw | 키워드 분포 | `test_text_analyzer.cpp` |
| [x] | FI-F01~08 | Filters::fil | 필터 P0·F5 게이트 | `test_filters.cpp` |
| [x] | SE-R01~04 | Session | static 격리·참조 | `test_session.cpp` |
| [x] | CO-01~04 | Constants | init·카테고리 | `test_constants.cpp` |
| [x] | CSV-R02,R04,ESC | CSV | 업로드/다운로드 현재 동작 | `test_csv_behavior.cpp` |
| [x] | HTTP-H01~04 | HTTP | 스모크 라우트 | `test_http_routes.cpp` |
| [x] | HTTP-H04b | HTTP | 업로드 후 통계 미표시 (BUG-P1-02) | `test_http_exceptions.cpp` |
| [x] | PERF-01 | 성능 | 1000+ 피드백·10s 이내 | `test_performance.cpp` |
| [x] | EX-01~05 | HTTP | 예외·invalid sentiment | `test_http_exceptions.cpp` |
| [x] | LG-01~05 | Logger | log smoke·debug 모드 | `test_logger.cpp` |
| [x] | UI-01~02 | UIComponents | CAT golden 5종 | `test_ui_components.cpp` |
| [x] | FH-01~02 | FileHandler | 스텁 stdout | `test_file_handler.cpp` |
| [x] | HTML-01~02 | HTML | escapeHtml·apostrophe | `test_html_escape.cpp` |
| [x] | FI-P2-02 | Filters | fil stdout 부작용 | `test_filters.cpp` |
| [x] | CSV-NH01 | CSV | 헤더 없음 1행 유실 | `test_csv_behavior.cpp` |
| [x] | coverage | gcov/lcov | `-DENABLE_COVERAGE=ON` → `feedback_analyzer_coverage` | `tests/CMakeLists.txt` |
| [x] | CI | ctest/GitHub Actions | `known-fail` 분리 job | `.github/workflows/tests.yml` |

**알려진 결함 테스트 (`[known-fail]`)** — 통과 = 현재 레거시 동작 고정, 수정 후 기대값 갱신 필요:

- TA-S04 (긍·부정 혼재 → 긍정)
- FI-F05 (품질 필터 main 스킵)
- FI-F06 (중립 필터 ≠ 통계)
- FI-F07b (`늦었어요` vs `늦다` 부분문자열 불일치)
- CSV-R02 (`id,text` → 첫 컬럼만 저장)
- CSV-NH01 (헤더 없는 CSV → 첫 행 유실)
- HTTP-H04b (업로드 후 HTML 통계 비어 있음)
- HTML-02 (apostrophe 미이스케이프)

**리팩토링 게이트 (녹색 전환 예정):** FI-F08, TA-S07/S08 `[post-phase2]`, CSV-R02 Phase 3 — [test_plan.md §8](docs/test_plan.md)
