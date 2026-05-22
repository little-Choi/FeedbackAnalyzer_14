# 작업 보고서 — Phase 0~7 리팩토링 구현

- **보고서 번호**: 010
- **작성일**: 2026-05-22
- **브랜치**: refactoring
- **커밋(작성 시점)**: dc2892d

## 1. 작업 개요

[docs/refactoring_plan.md](../docs/refactoring_plan.md)에 정의된 **Phase 0~7** 리팩토링을 한 세션에서 순차 적용했습니다. 각 Phase 완료 후 `cmake --build build` 및 `ctest --test-dir build`로 Green을 유지했고, Phase별 결과를 [docs/refactoring_result.md](../docs/refactoring_result.md)에 기록했습니다. God `main.cpp`를 `FeedbackApp`·라우트 핸들러·`HtmlPageRenderer`로 분리하고, `FeedbackRepository`로 상태를 통합했으며, `SentimentClassifier`·`CategoryMatcher`·`AppConfig` 등 모듈을 추가했습니다. `FileHandler` 스텁은 YAGNI로 삭제하여 테스트는 76건에서 74건으로 조정되었습니다(FH-01/02 제거).

## 2. 변경·산출물 목록

| 구분 | 경로/항목 | 설명 |
|------|-----------|------|
| 신규 | `src/cpp/AppConfig.h` | HTTP_PORT, BOM, CSV 헤더, FILTER_ALL |
| 신규 | `src/cpp/Sentiment.h`, `SentimentClassifier.h` | enum + S4 분류 정책 |
| 신규 | `src/cpp/CategoryRegistry.h`, `CategoryMatcher.h` | main 키워드 단일 구현 |
| 신규 | `src/cpp/FeedbackRepository.cpp/h` | all / lastFiltered 저장 |
| 신규 | `src/cpp/FeedbackApp.cpp/h` | HTTP 라우트·핸들러 |
| 신규 | `src/cpp/HtmlPageRenderer.cpp/h`, `HtmlEscape.h`, `HttpFormUtils.h`, `PageViewModel.h` | HTML·폼 유틸 |
| 수정 | `src/cpp/main.cpp` | 진입점만 (~6줄) |
| 수정 | `src/cpp/Constants.*`, `Filters.h`, `TextAnalyzer.h`, `Session.h`, `UIComponents.*`, `CsvHelpers.h` | Phase 1~4 반영 |
| 수정 | `CMakeLists.txt`, `tests/CMakeLists.txt` | 신규 소스 링크, FH 테스트 제거 |
| 수정 | `tests/fixtures/DomainFixture.h`, `HttpFixture.h` | Repository 연동 |
| 삭제 | `src/cpp/FileHandler.h`, `tests/unit/test_file_handler.cpp` | YAGNI |
| 문서 | `docs/refactoring_result.md` | Phase별 결과·검증 요약 |
| 보고서 | `Report/010-20260522-refactoring-implementation.md` | 본 문서 |
| 프롬프트 | `Prompting/010-...-Prompt.md`, `Prompting_user/010-...-User.md` | 대화 저장 |

### git diff 요약 (HEAD 대비, 24 files)

- **+140 / -566** lines (주로 `main.cpp` 이전·모듈 분리)
- `build/`, `.github/workflows/` — 본 커밋에서 **제외** (008 보고서와 동일: untracked 로컬 산출물)

## 3. 주요 결정·이슈

- **결정**: 테스트 Green 유지 — UI-01 실패 시 `categoryNames()`에 **명시적 표시 순서** 적용(std::map 순회는 한글 lexicographic).
- **결정**: CO-04 호환 — 감성 키워드 중복 블록 제거 후 `좋아요` 1건 중복 유지.
- **결정**: Phase 6-2 CSV 이스케이프 **미적용** — §5 계약·`CSV-ESC` 유지.
- **결정**: `sent`/`kw`/`fil` 레거시 alias 유지 — 테스트·HttpFixture 호환.
- **이슈(미해결)**: `.github/workflows/tests.yml` untracked — 별도 PR/커밋 권장.
- **이슈**: Golden Master 갱신 불필요 — 동작·stdout 동일 확인.

## 4. 테스트·검증

```text
cmake --build build
ctest --test-dir build --output-on-failure
→ 100% tests passed, 0 tests failed out of 74
```

| 구간 | 결과 |
|------|------|
| Phase 0 (시작) | 76/76 Passed |
| Phase 1 UI-01 수정 후 | 74/74 Passed |
| 최종 | 74/74 Passed |

선택 검증: `[p0]`, `GM-*`, `HTTP-*`, `CSV-*` — 모두 Passed.

## 5. 다음 단계

1. 리팩토링 소스를 **커밋 단위(Phase 1-1, 2-1 …)** 로 분할해 히스토리 정리(선택).
2. `.github/workflows/tests.yml` 추적·CI에 74 tests 반영.
3. 레거시 alias(`sent`/`kw`/`fil`) 제거는 테스트 마이그레이션 후 별도 PR.
