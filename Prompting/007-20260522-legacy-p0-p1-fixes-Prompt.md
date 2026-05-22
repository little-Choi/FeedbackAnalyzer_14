# 대화 프롬프트 기록 — 007-20260522-legacy-p0-p1-fixes

- **보고서 번호**: 007
- **저장일**: 2026-05-22

---

## Turn 1

**User**

@README.md @docs/qa_analysis.md @docs/test_plan.md @project_purpose.md

[P] 코드 개발 및 테스트에 강한 시니어 C++ QA입니다.
[C] C++17, catch2, CMake
[T] legacy 코드를 수정해줘.
    - @README.md 와 @docs/qa_analysis.md 를 참조
    - 모든 테스트 코드가 Green이 되게 작성
[F] 완성된 코드. 각 테스트 케이스 @README.md 에 추가하여 Green 시 체크박스 체크. 코드 수정 내용 요약하여@README.md 에 추가

**Assistant**

legacy 코드를 QA 분석과 테스트 기준에 맞춰 수정하고, 모든 테스트를 Green으로 만든 뒤 README에 반영하겠습니다.

- README, qa_analysis, test_plan, project_purpose 및 `src/cpp/`, `tests/` 구조 확인
- `build-full/tests/feedback_analyzer_tests.exe` 실행 → 64 cases 통과했으나 `[known-fail]`은 버그 동작 고정 상태임을 파악

**수정 (src/cpp/)**

1. **Filters**: `S_KEYWORDS` 제거, `Constants::SENTIMENT_KEYWORDS` 사용; 카테고리 필터 `main` 스킵 제거 → `kw`와 동일 규칙
2. **Constants**: 부정에 `늦`, `늦다` 추가
3. **main.cpp**: CSV `text` 컬럼·헤더 없음 1행; 업로드 후 `sent`/`kw`; `escapeHtml` `'` → `&#39;`

**테스트·헬퍼 동기화**

- `LegacyCsv.h`, `LegacyHtml.h`, `HttpFixture.h` 갱신
- FI-F05/F06/F07b, CSV-R02/NH01, HTTP-H04b, HTML-02, TA-S07 등 `[known-fail]` 해제·기대값 수정

**검증**

```text
cmake --build build-full
feedback_analyzer_tests.exe → All tests passed (135 assertions in 64 test cases)
```

**README**

- Test To Do → 케이스별 체크박스 64항 [x]
- 레거시 수정 요약 표 추가
- “레거시 비수정” 문구 제거

---

## Turn 2

**User**

'보고서 작성 및 저장 Agent' 실행해줘

**Assistant**

`.cursor/agent/report-github-manager.md` 4단계 워크플로 실행 — Report 007, Prompting, Prompting_user 작성 및 origin push.
