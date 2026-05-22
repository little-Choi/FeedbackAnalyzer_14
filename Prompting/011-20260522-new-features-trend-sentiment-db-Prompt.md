# 대화 프롬프트 기록 — 011-20260522-new-features-trend-sentiment-db

- **보고서 번호**: 011
- **저장일**: 2026-05-22

> 도구 호출은 `[REDACTED]`로 요약했습니다. Assistant 최종 답변은 가능한 한 전문을 유지합니다.

---

## Turn 1

**User**

@README.md @project_purpose.md 

[P] 테스트와 리팩토링을 잘하는 C++ 코드 개발자입니다.
[T] @project_purpose.md 에 추가 요구사항을 확인해서 코드 개발
   - 테스트가 Green이 유지되는 것을 계속 확인
[F] 기능 구현 완성된 코드 및 기능 구현 결과 내용 docs 폴더에 new_feature.md 파일에 저장

**Assistant**

`project_purpose.md` §6.1 항목 7의 추가 요구사항을 구현했고, **82개 테스트 전부 Green**입니다. 결과는 [docs/new_feature.md](docs/new_feature.md)에 정리했습니다.

## 구현 내용

### 1. 감정 트렌드 시각화
- CSV에 `date` + `text` 컬럼이 있으면 날짜별 긍정/중립/부정 스택 막대 차트를 대시보드에 표시합니다.
- 샘플: `data/test_feedback_trend.csv` (8건)
- **「트렌드 샘플 CSV 로드」** 버튼 → `POST /trend/load-sample`

### 2. 감정 키워드 File DB
- `data/sentiment_keywords.db` (탭 구분 텍스트)에 감정 키워드를 저장합니다.
- 서버 시작 시 로드, 없으면 `Constants` 기본값으로 파일을 생성합니다.
- 웹 UI **「감정 키워드 DB 관리」**에서 추가/삭제 → `POST /admin/sentiment`

### 3. 테스트 (신규 8건)
| ID | 내용 |
|----|------|
| TR-01~04 | CSV date 파싱, 트렌드 집계, 차트 HTML |
| DB-01~03 | File DB save/load, add/remove |
| HTTP-TR01 | date CSV 업로드 시 `trend:yes` |

## 주요 추가/변경 파일
- `src/cpp/TrendAnalyzer.h`, `SentimentKeywordDb.h`
- `src/cpp/Feedback.h`, `CsvHelpers.h`, `FeedbackApp.cpp`, `HtmlPageRenderer.cpp`
- `data/test_feedback_trend.csv`
- `tests/unit/test_trend_analyzer.cpp`, `test_sentiment_keyword_db.cpp`
- `tests/integration/test_trend_http.cpp`

## 실행
```bash
cmake --build build
build\feedback_analyzer.exe
ctest --test-dir build --output-on-failure
```

---

## Turn 2

**User**

'보고서 작성 및 저장 Agent' 실행해줘

**Assistant**

Report & GitHub Manager Agent 워크플로(011번) 실행 — `Report/011-...`, `Prompting/011-...`, `Prompting_user/011-...` 생성 및 git push.
