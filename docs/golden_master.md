# Golden Master (Approval) 회귀 테스트

TexttestFixture 스타일의 **출력 기반** 회귀 테스트입니다. HTTP 응답 본문, CSV 다운로드, `Filters::fil` stdout, 감성·키워드 직렬화 결과를 `tests/expected/`에 저장하고 Catch2에서 바이트 단위로 비교합니다.

## 1. expected 출력 파일 생성·보관 전략

| 항목 | 정책 |
|------|------|
| **저장 위치** | `tests/expected/{domain,csv,http}/` — 소스 트리에 커밋 |
| **경로 해석** | CMake가 `GOLDEN_EXPECTED_DIR` 매크로로 절대 경로 주입 (빌드 디렉터리와 무관) |
| **파일 형식** | `.txt`, `.html`, `.csv` — 실제 출력 그대로 (UTF-8) |
| **정규화** | 비교 시 CRLF→LF, 파일 끝 개행 1개로 통일 (`GoldenMaster.h`) |
| **갱신** | 의도된 동작 변경 시에만 baseline 갱신; PR에 diff 포함 |
| **금지** | `build/` 산출물, 타임스amp, 포트 번호는 golden에 넣지 않음 (HttpFixture는 stats HTML만 고정) |

### 디렉터리 예

```
tests/expected/
├── domain/
│   ├── TA_sent_kw_three_feedbacks.txt
│   └── FI_fil_stdout_positive.txt
├── csv/
│   ├── R04_empty_download.csv
│   ├── H03_filtered_download.csv
│   └── R02_upload_parsed.txt
└── http/
    ├── H01_analyze_positive.html
    ├── H02_filter_no_data.html
    ├── H03_filter_stats.html
    ├── H03_download.csv
    └── H04_upload_stats.html
```

### Baseline 갱신 절차

1. 프로덕션/테스트 픽스처 코드 변경
2. `FEEDBACK_ANALYZER_UPDATE_GOLDEN=1`로 golden 테스트만 실행
3. `tests/expected/` diff 리뷰 후 커밋

```powershell
.\scripts\update_golden.ps1
# 또는
cmake --build build --target update_golden
```

```bash
./scripts/update_golden.sh
```

## 2. Catch2 파일 비교 구현

- **헬퍼**: `tests/fixtures/GoldenMaster.h` — `golden::assertMatchesGolden(relativePath, actual)`
- **직렬화**: `tests/fixtures/GoldenSerialize.h` — map/피드백 목록을 안정적인 텍스트로 변환
- **테스트**: `tests/golden/test_golden_outputs.cpp` — 태그 `[golden][regression]`
- **실패 시**: 누락 파일 안내, 라인 단위 diff `INFO`

환경 변수:

| 변수 | 값 | 동작 |
|------|-----|------|
| `FEEDBACK_ANALYZER_UPDATE_GOLDEN` | `1` | expected 파일 덮어쓰기 (WARN) |
| (미설정) | — | strict 비교, 불일치 시 `FAIL_CHECK` |

## 3. CMake / ctest 통합

- `tests/CMakeLists.txt`: `golden/test_golden_outputs.cpp` 링크, `GOLDEN_EXPECTED_DIR` 정의
- `catch_discover_tests`: 각 `TEST_CASE`가 개별 `ctest` 항목으로 등록 (`ADD_TAGS_AS_LABELS`)
- **커스텀 타깃**: `update_golden` — golden 태그만 UPDATE 모드로 실행

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --target feedback_analyzer_tests

# 전체 (golden 포함)
ctest --test-dir build --output-on-failure

# Golden Master만 (Catch2 태그 필터 권장)
build/tests/feedback_analyzer_tests.exe "[golden]"
# ctest 이름 접두사 (Catch discover 시 등록됨)
ctest --test-dir build -R "GM-" --output-on-failure

# Baseline 갱신
cmake --build build --target update_golden
```

## 4. CI 자동 실행

`.github/workflows/tests.yml`의 `regression` job에서 `"[golden]"` 필터로 golden 테스트를 명시 실행합니다. `~[known-fail]` 전체 실행과 함께 수행되어 회귀 시 CI가 실패합니다.

## 5. test_plan / qa_analysis 매핑

| Golden ID | test_plan / qa | 검증 출력 |
|-----------|----------------|-----------|
| GM-TA | TA-S06, K3 | sent/kw map 직렬화 |
| GM-FI | FI-P2-02 | `fil` stdout |
| GM-CSV | CSV-R02, R04, H03 | 파싱·다운로드 CSV |
| GM-HTTP | HTTP-H01~H04 | in-process HTTP 본문 |

리팩토링 Phase 후 baseline만 `update_golden`으로 갱신하고, 단위 테스트 ID(FI-F05 등)는 그대로 유지합니다.
