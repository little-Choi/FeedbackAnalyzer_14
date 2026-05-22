# 작업 보고서 — Catch2 테스트 전량 구현

- **보고서 번호**: 006
- **작성일**: 2026-05-22
- **브랜치**: red
- **커밋(작성 시점)**: 362e229 (보고서 작성 전)

## 1. 작업 개요

[docs/test_plan.md](../docs/test_plan.md)를 기준으로 Catch2 v3·CMake 기반 `tests/` 스캐폴딩을 추가하고, 도메인 단위·CSV·HTTP 통합 테스트를 구현했습니다. 레거시 `src/cpp/`는 수정하지 않았으며, P0/P1 알려진 결함은 `[known-fail]` 태그로 기준선을 고정했습니다. 이후 미구현 항목(PERF-01, EX-01~05, Logger, UI, FileHandler, HTML escape, coverage 타깃, CI)까지 보완하여 **64 test cases, 136 assertions** 전부 통과를 확인했습니다.

## 2. 변경·산출물 목록

| 구분 | 경로/항목 | 설명 |
|------|-----------|------|
| 신규 | `tests/` | unit 9파일 + integration 3파일, fixtures, CMake, Catch2 FetchContent |
| 신규 | `tests/fixtures/DomainFixture.h` | Constants/Filters init, Session clear |
| 신규 | `tests/fixtures/HttpFixture.h` | in-process HTTP 스모크·예외 재현 (`__TEST_THROW__`) |
| 신규 | `tests/integration/LegacyCsv.h`, `LegacyHtml.h` | main.cpp 로직 복제(레거시 비수정) |
| 수정 | `CMakeLists.txt` | `BUILD_TESTING`, `add_subdirectory(tests)` |
| 수정 | `README.md` | 테스트 빌드·실행, Test To Do 전체 [x], known-fail 목록 |
| 신규 | `.github/workflows/tests.yml` | regression / known-fail / coverage CI |
| 옵션 | `feedback_analyzer_coverage` | `-DENABLE_COVERAGE=ON` 시 gcov 타깃 |
| 미변경 | `src/cpp/` | 레거시 프로덕션 코드 비수정 |
| 제외 | `build/`, `build-test/`, `build-full/` | 로컬 빌드 산출물 |

### 테스트 ID 커버리지 요약

| 영역 | ID |
|------|-----|
| Feedback | FB-01~04 |
| TextAnalyzer | TA-S01~08, TA-K01~05 |
| Filters | FI-F01~08, FI-F07b, FI-P2-02 |
| Session | SE-R01~04 |
| Constants | CO-01~04 |
| CSV | CSV-R02, R04, ESC, NH01 |
| HTTP | HTTP-H01~04, H04b |
| 예외 | EX-01~05 |
| 성능 | PERF-01 |
| Logger | LG-01~05 |
| UI | UI-01~02 |
| FileHandler | FH-01~02 |
| HTML | HTML-01~02 |

## 3. 주요 결정·이슈

- **결정**: `main.cpp` 미링크 — 도메인 `.cpp` + 테스트 전용 HTTP/CSV 복제.
- **결정**: EX-01/04는 레거시가 예외를 거의 던지지 않아 `HttpTestServer`에 `__TEST_THROW__` 마커로 catch 경로 검증.
- **결정**: FI-F07 vs FI-F07b — `늦었어요`/`늦다` 부분문자열 불일치를 별도 케이스로 문서화.
- **결정**: ctest 128건 중복은 `feedback_analyzer_coverage`의 `catch_discover_tests` 제거로 해소(64건).
- **이슈(해결)**: FI-P2-02 `CoutSilencer::captured()` 참조 오류 → 직접 `ostringstream` 캡처로 수정.
- **이슈(해결)**: ctest 한글 테스트명 인코딩 → TEST_CASE 제목 ASCII화.

## 4. 테스트·검증

```text
cmake -S . -B build-full -DBUILD_TESTING=ON -DENABLE_COVERAGE=ON
cmake --build build-full
feedback_analyzer_tests.exe  → 64 test cases, 136 assertions, All passed
ctest --test-dir build-full  → 64/64 passed
```

- known-fail 분리: `feedback_analyzer_tests.exe "[known-fail]"` / `"~[known-fail]"`

## 5. 다음 단계

1. MSYS2 `lcov`로 line coverage 90% 측정·`build/coverage_html` 리포트 생성.
2. 리팩토링 Phase 2 후 FI-F08·TA-S07/S08 기대값 갱신 및 `[known-fail]` 해제.
3. `main.cpp` 라우트 추출 시 HTTP 통합 테스트를 복제본 대신 실제 링크로 전환.
