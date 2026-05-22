# 작업 보고서 — 추가 요구사항 (트렌드·감정 키워드 File DB)

- **보고서 번호**: 011
- **작성일**: 2026-05-22
- **브랜치**: feature
- **커밋(작성 시점)**: 40e8220 (구현 변경은 본 커밋에 포함 예정)

## 1. 작업 개요

[project_purpose.md](../project_purpose.md) §6.1 항목 7의 **추가 요구사항**을 구현했습니다. `date` 컬럼이 포함된 CSV로 날짜별 감정 트렌드를 시각화하고, 감정 분석 키워드를 탭 구분 텍스트 File DB(`data/sentiment_keywords.db`)로 영속화·웹 UI에서 관리할 수 있게 했습니다. `data/test_feedback_trend.csv` 샘플과 **트렌드 샘플 CSV 로드** 버튼을 제공하며, 구현 결과는 [docs/new_feature.md](../docs/new_feature.md)에 정리했습니다. 전 과정에서 `ctest` Green(최종 **82/82**)을 유지했습니다.

## 2. 변경·산출물 목록

| 구분 | 경로/항목 | 설명 |
|------|-----------|------|
| 신규 | `src/cpp/TrendAnalyzer.h` | 날짜별 감정 집계·CSS 차트 HTML |
| 신규 | `src/cpp/SentimentKeywordDb.h` | File DB load/save/add/remove |
| 신규 | `data/test_feedback_trend.csv` | 트렌드 샘플 8건 |
| 신규 | `data/sentiment_keywords.db` | (런타임 생성) 감정 키워드 DB |
| 수정 | `src/cpp/Feedback.h` | `date` 필드, `hasDate()` |
| 수정 | `src/cpp/CsvHelpers.h` | `date` 컬럼 파싱, `csvUploadFeedbackRows` |
| 수정 | `src/cpp/FeedbackApp.cpp` | `/trend/load-sample`, `/admin/sentiment` |
| 수정 | `src/cpp/HtmlPageRenderer.cpp` | 트렌드 차트·키워드 DB 관리 UI |
| 수정 | `src/cpp/PageViewModel.h`, `AppConfig.h` | 트렌드·DB 뷰모델·경로 상수 |
| 신규 테스트 | `tests/unit/test_trend_analyzer.cpp` | TR-01~04 |
| 신규 테스트 | `tests/unit/test_sentiment_keyword_db.cpp` | DB-01~03 |
| 신규 테스트 | `tests/integration/test_trend_http.cpp` | HTTP-TR01 |
| 수정 | `tests/fixtures/HttpFixture.h` | 업로드 응답 `trend:yes/no` |
| 수정 | `tests/expected/http/H04_upload_stats.html` | `;trend:no` 반영 |
| 문서 | `docs/new_feature.md` | 기능 구현 결과 |
| 보고서 | `Report/011-20260522-new-features-trend-sentiment-db.md` | 본 문서 |

### git diff 요약 (HEAD 대비, 구현 파일)

- **+236 / -10** lines (9 tracked files; 신규 .h·테스트·data는 untracked → 본 커밋에 add)
- `build/`, `.github/workflows/` — **제외** (로컬·CI 산출물)

## 3. 주요 결정·이슈

- **결정**: File DB 형식은 외부 의존 없이 `감정<TAB>키워드` 텍스트 파일 사용.
- **결정**: 트렌드는 서버 사이드 CSS 스택 막대 차트(외부 JS/차트 라이브러리 없음).
- **결정**: 기존 `text` 단독 CSV·감성 S4 규칙·필터 AND 로직은 변경 없음.
- **이슈(해결)**: TR-04 — `Constants::init()` 미호출 → `DomainFixture` 적용.
- **이슈(해결)**: DB-01 — save 전 키워드 변경 순서 수정; Golden H04에 `;trend:no` 추가.
- **이슈(미해결)**: `.github/workflows/tests.yml` untracked — 별도 커밋 권장.

## 4. 테스트·검증

```text
cmake --build build
ctest --test-dir build --output-on-failure
→ 100% tests passed, 0 tests failed out of 82
```

| ID | 내용 | 결과 |
|----|------|------|
| TR-01~04 | CSV date, 트렌드 집계, 차트 HTML | Passed |
| DB-01~03 | File DB roundtrip, add/remove | Passed |
| HTTP-TR01 | date CSV 업로드 `trend:yes` | Passed |
| GM-HTTP H04 | golden `;trend:no` | Passed |
| 기존 74+ | 회귀 | Passed |

## 5. 다음 단계

1. `feature` 브랜치에 구현·보고서 커밋 후 PR 생성.
2. README 테스트 표(64 cases)를 82 cases로 갱신(선택).
3. 팀 리뷰·발표용 트렌드/DB 데모 시나리오 정리.
