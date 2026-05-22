# 작업 보고서 — Golden Master(Approval) 회귀 테스트

- **보고서 번호**: 008
- **작성일**: 2026-05-22
- **브랜치**: refactoring
- **커밋(작성 시점)**: 9f18e49

## 1. 작업 개요

[docs/test_plan.md](../docs/test_plan.md) 2차 항목(HTML·출력 golden)과 [docs/qa_analysis.md](../docs/qa_analysis.md)의 golden string 요구를 반영하여, TexttestFixture 스타일 **출력 기반 Golden Master** 회귀 테스트를 Catch2·CMake·CI에 통합했습니다. HTTP 응답 본문, CSV 다운로드, `Filters::fil` stdout, 감성·키워드 직렬화 결과를 `tests/expected/`에 baseline으로 저장하고 파일 비교합니다. 로컬에서 golden 9건·전체 76 test cases Green을 확인했습니다.

## 2. 변경·산출물 목록

| 구분 | 경로/항목 | 설명 |
|------|-----------|------|
| 신규 | `tests/fixtures/GoldenMaster.h` | `assertMatchesGolden`, CRLF 정규화, `FEEDBACK_ANALYZER_UPDATE_GOLDEN` 갱신 모드 |
| 신규 | `tests/fixtures/GoldenSerialize.h` | sent/kw map·텍스트 라인 직렬화 |
| 신규 | `tests/golden/test_golden_outputs.cpp` | GM-TA/FI/CSV/HTTP 9 cases, 태그 `[golden][regression]` |
| 신규 | `tests/expected/` | domain 2, csv 3, http 5 baseline 파일 |
| 신규 | `docs/golden_master.md` | 전략·실행·CI·test_plan 매핑 |
| 신규 | `scripts/update_golden.ps1`, `update_golden.sh` | baseline 일괄 갱신 |
| 수정 | `tests/CMakeLists.txt` | golden 소스·`GOLDEN_EXPECTED_DIR`·`update_golden` 타깃 |
| 수정 | `README.md` | Golden Master 실행·갱신 안내 |
| 로컬만 | `.github/workflows/tests.yml` | regression job `[golden]` 단계 — PAT `workflow` scope 없어 push 제외 |
| 신규 | `Report/008-...`, `Prompting/008-...`, `Prompting_user/008-...` | 본 보고서·대화 저장 |
| 제외 | `build/`, `build-cov/` | 로컬 빌드 산출물 |

### Golden 테스트 ID

| ID | 검증 출력 |
|----|-----------|
| GM-TA | `sent`/`kw` 3건 혼합 분포 |
| GM-FI | `fil` stdout (긍정 필터) |
| GM-CSV | R04 빈 다운로드, H03 1행 CSV, R02 파싱 라인 |
| GM-HTTP | H01~H04 응답 HTML·H03 download CSV |

## 3. 주요 결정·이슈

- **결정**: expected 파일은 소스 트리 `tests/expected/`에 커밋, CMake `GOLDEN_EXPECTED_DIR`로 절대 경로 주입.
- **결정**: baseline 갱신은 `FEEDBACK_ANALYZER_UPDATE_GOLDEN=1` 또는 `update_golden` 타깃/스크립트만 허용.
- **결정**: ctest 라벨 `golden`은 Catch discover 타이밍 이슈로 미사용; `ctest -R "GM-"` 또는 exe `"[golden]"` 필터 권장.
- **이슈**: GM-HTTP H03 실행 시 `fil`이 stdout에 피드백 텍스트 출력 — golden 비교에는 영향 없음, CI 로그에만 노출.

## 4. 테스트·검증

```text
cmake --build build --target feedback_analyzer_tests
$env:FEEDBACK_ANALYZER_UPDATE_GOLDEN=1
build\tests\feedback_analyzer_tests.exe "[golden]"  → baseline 10파일 생성

build\tests\feedback_analyzer_tests.exe "[golden]"  → 9 cases passed
build\tests\feedback_analyzer_tests.exe "~[known-fail]"  → 76 cases passed
ctest --test-dir build -R "GM-"  → 9/9 passed
cmake --build build --target update_golden  → 성공
```

## 5. 다음 단계

1. [docs/test_plan.md](../docs/test_plan.md)에 Golden Master 절(§2.4) 교차 참조 추가.
2. 리팩토링 후 HTML `renderPage` golden 확장(전체 페이지 스냅샷).
3. gcov gate와 Golden을 동일 CI job에서 `ctest` 일괄 실행.
