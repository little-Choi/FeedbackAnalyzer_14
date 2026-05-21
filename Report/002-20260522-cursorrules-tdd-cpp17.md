# 작업 보고서 — Cursor AI 규칙(.cursorrules) 작성

- **보고서 번호**: 002
- **작성일**: 2026-05-22
- **브랜치**: main
- **커밋(작성 시점)**: 97be72e

## 1. 작업 개요

Feedback Analyzer C++17 TDD 프로젝트에서 Cursor AI가 일관되게 따를 규칙을 프로젝트 루트 `.cursorrules` 파일로 정리했습니다. README.md, `project_purpose.md`, `CMakeLists.txt`를 참고해 기술 스택·모듈 구조·TDD 워크플로·Catch2 테스트 규칙·리팩토링·Git 정책을 문서화했습니다. 사용자 요청에 따라 보고서·프롬프트 기록을 저장하고 원격 저장소에 반영합니다.

## 2. 변경·산출물 목록

| 구분 | 경로/항목 | 설명 |
|------|-----------|------|
| 신규 | `.cursorrules` | C++17/CMake/Catch2, Given-When-Then, TEST_CASE_METHOD, should_*_when_* 명명, 경계값, 테스트 20줄 제한, Green 후 리팩토링, 매직 넘버 상수화, git merge 금지 등 (약 157줄) |
| 신규 | `Report/002-20260522-cursorrules-tdd-cpp17.md` | 본 작업 보고서 |
| 신규 | `Prompting/002-20260522-cursorrules-tdd-cpp17-Prompt.md` | 세션 전체 대화 기록 |
| 신규 | `Prompting_user/002-20260522-cursorrules-tdd-cpp17-User.md` | User 프롬프트만 기록 |
| 제외 | `build/` | 로컬 빌드 산출물 — 커밋 대상 아님 |

## 3. 주요 결정·이슈

- **결정**: Google Test `TEST_F` 요구는 Catch2 **`TEST_CASE_METHOD`** + fixture 클래스 패턴으로 매핑해 규칙에 예시 코드 포함.
- **결정**: 프로젝트 맥락(모듈 구조, God Function·전역 상태 리팩토링 대상, MinGW 빌드, `build/` 미커밋)을 README·`project_purpose.md` 기준으로 `.cursorrules`에 반영.
- **결정**: `git merge` 금지 및 커밋/푸시는 사용자 명시 요청 시에만 수행하도록 Git 규칙 명시.
- **미해결**: Catch2 테스트 타깃은 아직 CMake에 연동되지 않음 — 규칙은 향후 `tests/` 추가 시 적용 예정.

## 4. 테스트·검증

- **git status**: `main`, `origin/main`과 동기. untracked — `.cursorrules`, `build/` (제외 예정).
- **git diff --stat**: 추적 중인 파일 변경 없음 (신규 untracked만 존재).
- **git log -3 --oneline**:
  - `97be72e` docs: 보고서 001 및 프롬프트 저장 (spec-branch-report-agent)
  - `dccfc45` Cursor AI 퀴즈 문제 docx 추가
  - `96fa14b` main branch 초기 세팅
- **빌드/ctest**: 본 세션에서 `.cursorrules` 작성만 수행 — 해당 없음.

## 5. 다음 단계

1. Catch2 테스트 타깃을 CMake에 연동하고 `tests/`에 `TextAnalyzer` 단위 테스트 추가.
2. `.cursorrules`에 정의한 Given-When-Then·`should_*_when_*` 명명으로 첫 Red 테스트 작성.
3. Green 유지하며 `TextAnalyzer`·`Filters` 중복·매직 넘버 리팩토링 진행.
