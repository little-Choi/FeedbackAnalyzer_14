# 대화 프롬프트 기록 — 009-20260522-refactoring-plan

- **보고서 번호**: 009
- **저장일**: 2026-05-22

---

## Turn 1

**User**

@README.md @docs/qa_analysis.md @project_purpose.md 

[P] 모던 C++ 리팩토링 코치입니다.
[C] 테스트 Green에서만 진행
[T] 리팩토링 계획을 단계별로 제안해줘.
    - @README.md @docs/qa_analysis.md @project_purpose.md 파일 참조
    - 조건 분기 축소/중복 제거 (커밋 단위로 쪼개기)
    - 타입/정책 분리 (전략 패턴/테이블 기반/함수 분해)
    - 매직 넘버 상수화와 C++17 스타일 개선
[F] 단계별 체크리스트 + 각 단계 검증 방법 (cmake --build && ctest)

**Assistant**

문서·코드베이스 검토 후 **76/76 Green** 전제의 Phase 0~7 리팩토링 계획을 제안했습니다.

- **공통 게이트**: `cmake --build build` + `ctest --test-dir build --output-on-failure` (선택: `[p0]`, `GM-`, `TA-|FI-|SE-|CO-`)
- **현재 상태**: P0/P1 수정·`SentimentMatcher`·`CsvHelpers` 완료 / 남은 스멜: `sent`/`kw`/`fil`, Constants 중복, `fil_data`·Session, God `main`, FileHandler 스텁
- **Phase 1~7**: 커밋 단위 목표·기법·검증 테스트 표
- **동작 계약 6항**: S4 긍정 우선, FI-F08, main 키워드, CSV text, 빈 download, CSV-ESC 등

---

## Turn 2

**User**

제안 내용 docs폴더에 refactoring_plan.md 파일에 요약해서 저장해줘.

**Assistant**

`docs/refactoring_plan.md`에 요약 저장 완료 (공통 게이트, Phase 0~7, 의존성 순서, 동작 계약, 체크리스트, 관련 문서 링크).

---

## Turn 3

**User**

'보고서 작성 및 저장 Agent' 실행해줘

**Assistant**

Report & GitHub Manager 워크플로 실행 — `Report/009-...`, `Prompting/009-...`, `Prompting_user/009-...` 생성 및 git push.
