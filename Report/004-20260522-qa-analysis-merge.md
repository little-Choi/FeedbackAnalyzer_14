# 작업 보고서 — QA 분석·코드 스멜 문서 통합

- **보고서 번호**: 004
- **작성일**: 2026-05-22
- **브랜치**: spec
- **커밋(작성 시점)**: 10c30da

## 1. 작업 개요

레거시 Feedback Analyzer 코드베이스에 대해 QA 관점(기능 결함·명세 불일치) 정적 분석을 수행하고, 결과를 `docs/qa_analysis.md`에 저장했습니다. 이후 `docs/code_smell.md`와 `docs/qa_analysis.md`를 하나의 통합 문서(`docs/qa_analysis.md`)로 병합했고, 리다이렉트 스텁이었던 `docs/code_smell.md`를 삭제했습니다. 마지막으로 Report·Prompting·Prompting_user 기록 및 원격 저장소 반영을 수행합니다.

## 2. 변경·산출물 목록

| 구분 | 경로/항목 | 설명 |
|------|-----------|------|
| 신규 | `docs/qa_analysis.md` | 코드 스멜·QA 통합 분석 (목차 6장, Phase 1–7, S/K/F/R/H 테스트) |
| 삭제 | `docs/code_smell.md` | 통합 후 단일 참조 문서로 대체 (520줄 제거) |
| 신규 | `Report/004-20260522-qa-analysis-merge.md` | 본 작업 보고서 |
| 신규 | `Prompting/004-20260522-qa-analysis-merge-Prompt.md` | 세션 전체 대화 기록 |
| 신규 | `Prompting_user/004-20260522-qa-analysis-merge-User.md` | User 프롬프트만 기록 |
| 분석 대상 | `src/cpp/`, `project_purpose.md` | P0/P1/P2 결함·스멜 7건 식별 |
| 제외 | `build/` | 로컬 CMake 빌드 산출물 — 커밋 대상 아님 |

## 3. 주요 결정·이슈

- **결정**: QA(P0~P2)와 코드 스멜(7건)을 `docs/qa_analysis.md` 단일 문서로 통합, 스멜↔QA 매핑 표(§1.3)로 연결.
- **결정**: `docs/code_smell.md`는 통합 완료 후 삭제 — 중복·분산 참조 방지.
- **결정**: P0 우선 수정 항목 — `Filters::fil`의 `main` 스킵, 이중 감성 키워드 사전, 중립 필터 불일치.
- **미해결**: Catch2 테스트·실제 리팩토링은 미착수 — 통합 문서의 Phase·테스트 ID 정의만 존재.

## 4. 테스트·검증

- **git status (작성 시점)**: `spec`, `origin/spec` 동기. `deleted: docs/code_smell.md`, `untracked: docs/qa_analysis.md`, `build/`(제외).
- **git diff --stat**: `docs/code_smell.md` 520 deletions.
- **git log -3 --oneline**:
  - `10c30da` docs: 보고서 003 및 프롬프트 저장 (code-smell-analysis)
  - `78a860c` docs: 보고서 002 및 프롬프트 저장 (cursorrules-tdd-cpp17)
  - `97be72e` docs: 보고서 001 및 프롬프트 저장 (spec-branch-report-agent)
- **빌드/ctest**: 본 세션은 정적 분석·문서 작업만 — 해당 없음.

## 5. 다음 단계

1. `docs/qa_analysis.md` §5.1 F5·K2 기준으로 Catch2 Red 테스트 추가.
2. Phase 2(키워드 단일화) — P0 감성·필터 불일치 해소.
3. CSV `text` 컬럼 파싱 및 `fil_data` vs `Session` 다운로드 소스 통일.
