# 대화 프롬프트 기록 — 004-20260522-qa-analysis-merge

- **보고서 번호**: 004
- **저장일**: 2026-05-22 12:00

---

## Turn 1

**User**

@c:\DEV\FeedbackAnalyzer_14\ legacy 코드입니다. QA 작업을 하려고 합니다. 당신은 문제 발견 전문가입니다. 
@c:\DEV\FeedbackAnalyzer_14\ 에서 QA를 하기 위한 진짜 문제가 무엇인지 분석해서 알려주세요.

**Assistant**

레거시 **Feedback Analyzer**를 QA 관점에서 분석했습니다. `project_purpose.md`에 명시된 대로 **의도적인 코드 스멜·불일치가 포함된 리팩토링 연습용** 코드이며, 그 안에 **실제로 재현 가능한 기능 결함**이 여러 개 있습니다.

*(도구: Glob, Read README.md, code_smell.md, src/cpp 전체, project_purpose.md, Grep, CMakeLists.txt)*

### QA 핵심 요약

| 우선순위 | 문제 | 영향 |
|----------|------|------|
| **P0** | 카테고리 필터가 `main` 키워드를 무시함 | "품질" 등 필터가 통계와 반대로 동작 |
| **P0** | 감성 분석 vs 필터가 **서로 다른 키워드 사전** 사용 | 화면 숫자와 필터 결과 불일치 |
| **P0** | **중립** 필터·통계 불일치 (`괜찮` 등) | 교육 과정 "동작오류" 항목 |
| **P1** | CSV가 `text` 컬럼이 아니라 **첫 번째 컬럼**만 사용 | README 명세 위반 |
| **P1** | 다운로드는 **마지막 필터 결과만**, 필터 없으면 빈 파일 | 사용자 기대와 다름 |
| **P1** | CSV 업로드 후 **분석 통계 미표시** | 기능 누락 |
| **P2** | 자동화 테스트 **0건** | 회귀 검증 불가 |
| **P2** | 로그는 콘솔만, UI에 level별 미표시 | `project_purpose.md` 미완 |

### P0 — 반드시 잡아야 할 기능 결함

1. **카테고리 필터 `main` 스킵** — `TextAnalyzer::kw`는 `main`만 사용, `Filters::fil`은 `main`을 `continue`로 건너뜀. 재현: `품질이 좋습니다` → analyze 품질:1 → filter 키워드=품질 → 0건.
2. **이중 감성 키워드 사전** — `Constants::SENTIMENT_KEYWORDS` vs `Filters::S_KEYWORDS`. 재현: `괜찮아요` 통계 중립+1, 필터 중립 0건; `배송이 늦었어요` 통계 중립, 필터 부정 포함.
3. **긍·부정 혼재 시 항상 긍정** — `sent()` if-else 순서. 예: `좋은데 나쁜 부분도 있음` → 긍정.

### P1 — 명세·UX

- CSV `id,text` → 첫 컬럼(id)만 저장, 헤더 무조건 스킵, 업로드 후 통계 `{}`
- `/download` → `fil_data` 전역만, 필터 없으면 빈 CSV, 이스케이프 없음, `FileHandler` 미사용
- 상태 3곳 분산: `Session`, `fil_data`, `globalSent/Kw`

### P2 — 인프라

- Catch2/`tests/` 없음, 피드백 초기화 API 없음, HTML 이스케이프·바인딩 등

### 권장 QA 테스트 플랜

스모크(품질·괜찮 필터), F5 일관성 표, CSV·다운로드(R2,H3), 경계 케이스.

### 결론

의도적 스멜은 리팩토링 대상이고, **진짜 QA 타깃**은 통계·필터·다운로드·CSV가 서로 다른 규칙/데이터를 쓰는 **P0/P1 기능 버그**입니다.

---

## Turn 2

**User**

해당 내용을 정리하여 docs폴더에 md파일로 저장해줘.

**Assistant**

QA 분석 내용을 `docs` 폴더에 마크다운 파일로 저장합니다.

*(도구: Write `docs/qa_analysis.md`)*

`docs/qa_analysis.md`에 QA 분석 내용을 저장했습니다.

포함 섹션: QA 핵심 요약, P0/P1/P2, 권장 QA 플랜, S/K/F/R/H 자동화 테스트 ID, 결론·수정 우선순위, 관련 문서 링크.

`docs/code_smell.md`와 역할 분리 — 스멜·리팩토링은 `code_smell.md`, QA·재현 시나리오는 `qa_analysis.md`.

---

## Turn 3

**User**

@docs/code_smell.md 과 @docs/qa_analysis.md 를 하나의 파일로 정리해서 @docs/qa_analysis.md  파일에 저장해줘.

**Assistant**

두 문서를 읽어 `docs/qa_analysis.md` 하나로 통합합니다.

*(도구: Read code_smell.md, qa_analysis.md; Write qa_analysis.md 통합본; Write code_smell.md 리다이렉트 스텁)*

`docs/qa_analysis.md`에 두 문서를 통합해 저장했습니다. `docs/code_smell.md`는 통합 문서로 안내하는 짧은 리다이렉트만 남겼습니다.

### 통합 문서 구조

| 섹션 | 내용 |
|------|------|
| 1. 통합 요약 | QA P0~P2, 코드 스멜 7건, 스멜↔결함 매핑 |
| 2. QA — 기능 결함 | P0/P1/P2 상세·재현 시나리오 |
| 3. 코드 스멜 상세 진단 | 기존 code_smell.md 2장 |
| 4. 리팩토링 제안 | Phase 1~7 |
| 5. 회귀 테스트·QA 플랜 | S/K/F/R/H, DoD, 수동 스모크 |
| 6. 결론·우선순위 | QA·리팩토링 통합 매트릭스 |

---

## Turn 4

**User**

@docs/code_smell.md 파일 지우고,  '보고서 작성 및 저장 Agent' 실행해줘

**Assistant**

`docs/code_smell.md`를 삭제하고, 보고서 작성 Agent 설정을 확인한 뒤 실행합니다.

*(도구: Delete docs/code_smell.md; Read report-github-manager.md; Glob Report/)*

→ report-github-manager 서브에이전트 실행 (본 저장 작업).
