# 대화 프롬프트 기록 — 002-20260522-cursorrules-tdd-cpp17

- **보고서 번호**: 002
- **저장일**: 2026-05-22

---

## Turn 1

**User**

[P] 당신은 레거시 코드 QA/리팩토링을 돕는 시니어 C++ 엔지니어입니다.
[C] TDD TV C++17 프로젝트에서 Cursor AI가 항상 따라야 하는 규칙을
    프로젝트 루트의 .cursorrules로 작성하려고 합니다.
[T] 아래 요구를 만족하는 .cursorrules 내용을 작성해줘. README.md 파일을 읽고 규칙을 추가해도 돼.
    - 기술 스택: C++17 + CMake + Catch2
    - 테스트 규칙: Given-When-Then, TEST_F, 경계값 포함, 테스트 메서드명은 should_[결과]_when_[조건] 형태, 한 함수는 최대 20줄
    - 리팩토링 규칙: 테스트 Green 상태에서만 진행, 매직 넘버 상수화 권장
    - git 규칙: git merge 진행하지 않기
[F] .cursorrules 파일을 만들기

**Assistant**

README와 프로젝트 구조를 확인한 뒤 `.cursorrules`를 작성합니다.

*(도구: README.md, 프로젝트 Glob, CMakeLists.txt, project_purpose.md, TextAnalyzer.h 등 읽기)*

`.cursorrules` 파일을 프로젝트 루트에 생성했습니다. README·`project_purpose.md`·`CMakeLists.txt` 내용을 반영했습니다.

### 포함된 규칙 요약

| 영역 | 내용 |
|------|------|
| **기술 스택** | C++17, CMake 3.14+, Catch2, cpp-httplib |
| **테스트** | Given–When–Then 주석, `TEST_CASE_METHOD`(TEST_F 대응), `should_[결과]_when_[조건]` 명명, 경계값·UTF-8·중립 필터, 테스트 본문 최대 20줄 |
| **TDD** | Red → Green → Refactor, `ctest` 실행 절차 |
| **리팩토링** | Green 상태에서만 진행, 매직 넘버 `Constants` 상수화, God Function·중복·전역 상태 개선 가이드, 커버리지 90% 목표 |
| **Git** | `git merge` 금지, 커밋/푸시는 사용자 요청 시에만 |
| **프로젝트** | 모듈 구조, CSV `text` 컬럼, MinGW 빌드 명령, `build/` 미커밋 |

### TEST_F와 Catch2

요구하신 `TEST_F`는 Catch2에서는 **`TEST_CASE_METHOD`**로 매핑해 두었습니다. Google Test와 동일한 fixture 패턴이며, `.cursorrules`에 예시 코드도 넣었습니다.

추가로 Google Test를 직접 쓰도록 바꾸거나, `.cursor/rules/` 형식으로 옮기길 원하시면 알려주세요.

---

## Turn 2

**User**

보고서 작성 및 저장 Agent 실행해줘

**Assistant**

보고서 작성 Agent 설정을 확인한 뒤 Report & GitHub Manager 워크플로를 실행합니다.

*(report-github-manager 서브에이전트 실행 — 보고서·프롬프트 저장 및 GitHub push)*
