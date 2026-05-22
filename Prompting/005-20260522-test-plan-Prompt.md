# 대화 프롬프트 기록 — 005-20260522-test-plan

- **보고서 번호**: 005
- **저장일**: 2026-05-22 12:00

---

## Turn 1

**User**

@README.md @docs/qa_analysis.md 

[P] 시니어 QA 리드입니다.
[C] C++17, catch2, CMake
[T] 테스트 계획서를 작성해줘. 테스트 실패여도 괜찮으니 legacy code는 수정하지 말아줘.
    - @README.md 및 @docs/qa_analysis.md 기반으로 작성
    - TEST_F 기반 단위 테스트 범위/우선순위
    - 경계값 케이스 목록
    - 예외/특이 케이스 목록
    - 커버리지 목표 (예: 90%+)와 gcov/lcov 측정·개선 전략
[F] Markdown 문서. docs 폴더에 test_plan.md로 저장

**Assistant**

README와 QA 분석 문서를 읽고 테스트 계획서를 작성합니다.

*(도구: Read README.md, qa_analysis.md; Glob src/cpp; Read CMakeLists.txt, TextAnalyzer.h, Filters.h, Feedback.h, project_purpose.md, Constants.h, Session.h, Grep main routes, Constants.cpp, Filters.cpp, Logger.h, main.cpp upload/download)*

`docs/test_plan.md`에 테스트 계획서를 저장했습니다. 레거시 `src/cpp/` 코드는 건드리지 않았습니다.

### 문서 구성 요약

1. **원칙** — 실패 허용, P0/P1 결함은 `[known-fail]`로 기준선 고정 (코드 수정 없음)
2. **TEST_F 범위·우선순위** — P0: `TextAnalyzer::sent/kw`, `Filters::fil` (FI-F05/F06/F07 등); P1: Session, HTTP/CSV; P2~P3: Constants, Logger, Feedback
3. **경계값** — §4: 0건·1건·대량, 빈 텍스트, UTF-8, 필터 `전체`, CSV 헤더/컬럼, `fil_data` 상태
4. **예외·특이** — §5: BUG-P0-01~03 등 qa_analysis 결함과 TEST_F 1:1 매핑
5. **커버리지 90%+** — §6: gcov/lcov 명령, 모듈별 목표, `main.cpp`/`FileHandler` 제외
6. **스프린트·리팩토링 게이트** — Phase 2 후 FI-F08(F5) 녹색, known-fail 해제 조건

`qa_analysis.md`의 S1–H4 ID는 §10 매핑표로 연결.

---

## Turn 2

**User**

'보고서 작성 및 저장 Agent' 실행해줘

**Assistant**

`.cursor/agent/report-github-manager.md` 워크플로 4단계 실행 — 보고서 005, Prompting, Prompting_user, git push.

*(도구: next-report-id.ps1 → 005; git status/branch/log; Write Report/005, Prompting/005, Prompting_user/005; git add/commit/push)*
