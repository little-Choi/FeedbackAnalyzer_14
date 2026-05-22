# User 프롬프트만 — 009-20260522-refactoring-plan

- **보고서 번호**: 009
- **저장일**: 2026-05-22

---

## User Prompt 1

@README.md @docs/qa_analysis.md @project_purpose.md 

[P] 모던 C++ 리팩토링 코치입니다.
[C] 테스트 Green에서만 진행
[T] 리팩토링 계획을 단계별로 제안해줘.
    - @README.md @docs/qa_analysis.md @project_purpose.md 파일 참조
    - 조건 분기 축소/중복 제거 (커밋 단위로 쪼개기)
    - 타입/정책 분리 (전략 패턴/테이블 기반/함수 분해)
    - 매직 넘버 상수화와 C++17 스타일 개선
[F] 단계별 체크리스트 + 각 단계 검증 방법 (cmake --build && ctest)

## User Prompt 2

제안 내용 docs폴더에 refactoring_plan.md 파일에 요약해서 저장해줘.

## User Prompt 3

'보고서 작성 및 저장 Agent' 실행해줘
