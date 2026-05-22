# 작업 보고서 — 모던 C++ 리팩토링 계획 수립 및 문서화

- **보고서 번호**: 009
- **작성일**: 2026-05-22
- **브랜치**: refactoring
- **커밋(작성 시점)**: cac6f5a

## 1. 작업 개요

사용자 요청에 따라 [README.md](../README.md), [docs/qa_analysis.md](./qa_analysis.md), [project_purpose.md](../project_purpose.md)를 참고하여 **테스트 Green 전제**의 모던 C++ 리팩토링 단계별 계획을 제안했습니다. 조건 분기 축소·중복 제거(커밋 단위), 타입/정책 분리(전략·테이블·함수 분해), 매직 넘버 상수화·C++17 스타일을 Phase 0~7로 정리했고, 각 단계별 `cmake --build` + `ctest` 검증 방법과 동작 계약 6항을 명시했습니다. 이어서 동일 내용을 `docs/refactoring_plan.md`에 요약 저장했습니다. 로컬 기준선은 76/76 tests passed를 확인했습니다.

## 2. 변경·산출물 목록

| 구분 | 경로/항목 | 설명 |
|------|-----------|------|
| 신규 | `docs/refactoring_plan.md` | Phase 0~7, 커밋 체크리스트, 공통 게이트, 동작 계약, 관련 문서 링크 |
| 신규 | `Report/009-20260522-refactoring-plan.md` | 본 보고서 |
| 신규 | `Prompting/009-20260522-refactoring-plan-Prompt.md` | 전체 대화 기록 |
| 신규 | `Prompting_user/009-20260522-refactoring-plan-User.md` | User 프롬프트만 |
| 참조만 | `src/cpp/` | P0/P1 수정·`SentimentMatcher`·`CsvHelpers` 완료 상태 기준으로 계획 작성 (본 세션 미수정) |
| 제외 | `build/`, `build-cov/` | 로컬 빌드 산출물 |

### 계획 요약 (Phase)

| Phase | 초점 | 커밋 수 |
|-------|------|---------|
| 0 | Green 기준선 고정 | — |
| 1 | Constants 중복·매직 넘버·UI 카테고리 단일화 | 3 |
| 2 | SentimentClassifier, fil 파이프라인, CategoryMatcher | 4 |
| 3 | Sentiment enum, CategoryRegistry, API rename | 3 |
| 4 | FeedbackRepository, fil_data 통합, stdout 정책 | 3 |
| 5 | main.cpp 핸들러·HtmlRenderer·FeedbackApp | 3 |
| 6 | FileHandler / CSV escape | 2 |
| 7 | string_view, nodiscard, optional | 2 |

## 3. 주요 결정·이슈

- **결정**: 리팩토링은 **한 커밋 = 한 축**, 매 커밋 후 전체 `ctest` Green 유지.
- **결정**: Golden Master(`GM-*`)는 stdout/HTML/CSV 출력 변경 시에만 `update_golden`으로 갱신.
- **결정**: 동작 계약(S4 긍정 우선, FI-F08 통계=필터, main 키워드 필터, CSV text 컬럼 등)은 Phase 진행 중 변경 금지.
- **이슈(미해결)**: `globalSent`/`globalKw`, `fil_data`, God `main`, `FileHandler` 스텁 — Phase 2~6에서 순차 처리 예정.
- **이슈**: `.github/workflows/tests.yml`은 untracked — 본 보고서 커밋 범위에서 제외(008 보고서와 동일).

## 4. 테스트·검증

```text
ctest --test-dir build --output-on-failure
→ 100% tests passed, 0 tests failed out of 76
```

본 세션에서는 소스 리팩토링 미수행 — 계획 문서만 추가.

## 5. 다음 단계

1. **Commit 1-1**: `Constants.cpp` 감성 키워드 중복 블록 제거 후 `CO-*`, `[p0]` 검증.
2. **Commit 2-1**: `SentimentClassifier` Extract Class (`TA-S*`, `FI-F*`).
3. `docs/test_plan.md`에 `refactoring_plan.md` 교차 참조 추가(선택).
