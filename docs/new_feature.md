# 추가 요구사항 구현 결과

| 항목 | 내용 |
|------|------|
| **작성일** | 2026-05-22 |
| **기준** | [project_purpose.md](../project_purpose.md) §6.1 항목 7 |
| **테스트** | 82 cases Green (`ctest --test-dir build`) |

---

## 1. 구현 요약

| 기능 | 설명 | 주요 파일 |
|------|------|-----------|
| **감정 트렌드 시각화** | `date` 컬럼이 있는 CSV 업로드 시 날짜별 긍정/중립/부정 스택 막대 차트 표시 | `TrendAnalyzer.h`, `CsvHelpers.h`, `HtmlPageRenderer.cpp` |
| **트렌드 샘플 로드** | `data/test_feedback_trend.csv` 원클릭 로드 | `FeedbackApp.cpp`, `data/test_feedback_trend.csv` |
| **감정 키워드 File DB** | 탭 구분 텍스트 파일로 감정 키워드 영속화·웹 UI 관리 | `SentimentKeywordDb.h`, `data/sentiment_keywords.db` |

---

## 2. 감정 트렌드 시각화

### 2.1 CSV 형식

`date`와 `text` 컬럼이 함께 있으면 트렌드 분석 대상으로 처리합니다.

```csv
date,text
2026-05-01,배송이 빠르고 좋습니다
2026-05-02,배송이 늦었어요
```

- 기존 `text` 단독 CSV는 이전과 동일하게 동작합니다.
- 샘플 파일: `data/test_feedback_trend.csv` (8건, 5일간 분포)

### 2.2 동작

1. CSV 업로드 또는 **「트렌드 샘플 CSV 로드」** 버튼 → 피드백에 `date` 저장
2. `TrendAnalyzer::analyzeByDate()`가 날짜별 감정 건수 집계
3. 대시보드 **「감정 트렌드 (날짜별)」** 섹션에 CSS 스택 막대 차트 렌더
   - 초록: 긍정, 회색: 중립, 빨강: 부정

### 2.3 HTTP 라우트

| 메서드 | 경로 | 설명 |
|--------|------|------|
| POST | `/trend/load-sample` | `data/test_feedback_trend.csv` 로드 |

### 2.4 테스트

| ID | 설명 |
|----|------|
| TR-01 | `date,text` CSV 파싱 |
| TR-02 | 날짜별 감정 집계 |
| TR-03 | 샘플 CSV 8행 로드 |
| TR-04 | 차트 HTML 마크업 생성 |
| HTTP-TR01 | 업로드 응답 `trend:yes` 플래그 |

---

## 3. 감정 키워드 File DB

### 3.1 파일 형식

경로: `data/sentiment_keywords.db` (앱 최초 실행 시 `Constants::init()` 기본값으로 자동 생성)

```
# sentiment keyword database (tab-separated)
긍정	좋아요
긍정	만족
부정	나쁘
...
```

- 한 줄 = `감정라벨<TAB>키워드`
- `#` 로 시작하는 줄은 주석

### 3.2 동작

1. 서버 시작: DB 파일이 있으면 `Constants::SENTIMENT_KEYWORDS` 로드, 없으면 기본값 저장
2. 대시보드 **「감정 키워드 DB 관리」** 섹션에서 목록 조회·추가·삭제
3. 변경 즉시 파일에 반영 → 이후 `SentimentClassifier` 분류에 적용

### 3.3 HTTP 라우트

| 메서드 | 경로 | 파라미터 |
|--------|------|----------|
| POST | `/admin/sentiment` | `action=add`, `sentiment`, `keyword` |
| POST | `/admin/sentiment` | `action=remove`, `sentiment`, `keyword` |

### 3.4 테스트

| ID | 설명 |
|----|------|
| DB-01 | save/load 라운드트립 |
| DB-02 | addKeyword 후 분류 반영 |
| DB-03 | removeKeyword 후 미매칭 → 중립 |

---

## 4. 변경된 모델·설정

### Feedback

- `getDate()`, `hasDate()` 추가 — 트렌드용 선택적 날짜 필드

### AppConfig

- `SENTIMENT_DB_PATH` = `data/sentiment_keywords.db`
- `TREND_SAMPLE_CSV_PATH` = `data/test_feedback_trend.csv`
- `CSV_DATE_HEADER` = `date`

---

## 5. 사용 방법

```bash
cmake --build build
build\feedback_analyzer.exe
```

1. 브라우저에서 `http://localhost:8080` 접속
2. **트렌드 샘플 CSV 로드** → 날짜별 감정 트렌드 차트 확인
3. **감정 키워드 DB 관리**에서 키워드 추가/삭제 후 피드백 분석·필터로 동작 확인

```bash
ctest --test-dir build --output-on-failure
# 신규 태그만
build\tests\feedback_analyzer_tests.exe "[trend]"
build\tests\feedback_analyzer_tests.exe "[sentiment-db]"
```

---

## 6. 회귀·호환성

- 기존 74+ Golden/단위 테스트 유지 (H04 golden에 `;trend:no` 반영)
- `text` 단독 CSV, 감성 분류 S4 규칙(긍정 우선), 필터 AND 로직 변경 없음
- 총 **82** test cases Green
