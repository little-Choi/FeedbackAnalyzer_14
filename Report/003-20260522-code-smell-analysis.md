# 작업 보고서 — 코드 스멜 분석 (src/cpp)

- **보고서 번호**: 003
- **작성일**: 2026-05-22
- **브랜치**: main
- **커밋(작성 시점)**: 78a860c

## 1. 작업 개요

사용자 요청에 따라 `src/cpp/` 전체를 Clean Code·리팩토링 관점에서 정적 분석했습니다. 최소 3개 이상의 코드 스멜, SOLID 위배 설명, 단계별 리팩토링 전략, 회귀 방지용 최소 유닛 테스트 시나리오를 `docs/code_smell.md`(521줄, 4개 섹션)에 저장했습니다. 이후 사용자가 커스텀 Report & GitHub Manager Agent(Report/, Prompting/, Prompting_user/, git push) 실행을 요청하여 본 보고서·프롬프트 기록 및 원격 반영을 수행합니다.

## 2. 변경·산출물 목록

| 구분 | 경로/항목 | 설명 |
|------|-----------|------|
| 신규 | `docs/code_smell.md` | 코드 스멜 7건, Phase 1–7 리팩토링, 테스트 S/K/F/R/H 시나리오 |
| 신규 | `Report/003-20260522-code-smell-analysis.md` | 본 작업 보고서 |
| 신규 | `Prompting/003-20260522-code-smell-analysis-Prompt.md` | 세션 전체 대화 기록 |
| 신규 | `Prompting_user/003-20260522-code-smell-analysis-User.md` | User 프롬프트만 기록 |
| 분석 대상 | `src/cpp/` | main, Session, TextAnalyzer, Filters, FileHandler, Constants, UIComponents, Logger, Feedback |
| 제외 | `build/` | 로컬 CMake/MinGW 빌드 산출물 — 커밋 대상 아님 |

## 3. 주요 결정·이슈

- **결정**: 식별 스멜 7건 — 축약 메서드명, 중복 키워드, Fake Session, 전역 상태, FileHandler 스텁, God Module(main), 비즈니스 로직 중복.
- **결정**: 리팩토링 우선순위 — 키워드 단일화 → SentimentClassifier 추출 → Session→Repository → FileHandler 정리 → HTML 분리.
- **이슈(해결)**: 첫 번째 "보고서 작성 및 저장" 요청 시 `docs/code_smell.md`만 완료된 것으로 오인 안내함. 사용자가 커스텀 agent 워크플로를 명시하여 본 003번 보고서·프롬프트·push로 정정.
- **미해결**: 실제 리팩토링·Catch2 테스트 구현은 미착수 — `docs/code_smell.md` Phase·테스트 정의만 존재.

## 4. 테스트·검증

- **git status (작성 시점)**: `main`, `origin/main` 동기. untracked — `docs/`, `build/`(제외).
- **git diff --stat**: 추적 중인 파일 변경 없음 (신규 untracked만).
- **git log -3 --oneline**:
  - `78a860c` docs: 보고서 002 및 프롬프트 저장 (cursorrules-tdd-cpp17)
  - `97be72e` docs: 보고서 001 및 프롬프트 저장 (spec-branch-report-agent)
  - `dccfc45` Cursor AI 퀴즈 문제 docx 추가
- **빌드/ctest**: 본 세션은 정적 분석·문서 작성만 — 해당 없음.

## 5. 다음 단계

1. `docs/code_smell.md` Phase 2(키워드 단일화) 전 F5 일관성 테스트를 Catch2로 Red 작성.
2. `SentimentClassifier` 추출 및 `sent`/`kw`/`fil` 명명 개선(Phase 1–4).
3. Session 제거 및 `FeedbackRepository` 도입 후 HTTP 핸들러에서 전역 상태 축소.
