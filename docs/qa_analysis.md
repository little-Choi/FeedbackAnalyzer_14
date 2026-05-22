# Feedback Analyzer — 코드 스멜·QA 통합 분석 보고서

**분석 대상:** `src/cpp/` (레거시 코드)  
**분석 일자:** 2026-05-22  
**관점:** Clean Code, Refactoring, SOLID, 기능 결함, 회귀 테스트

> 본 문서는 기존 `docs/code_smell.md` 와 `docs/qa_analysis.md` 를 통합한 단일 참조 문서입니다.

---

## 목차

1. [통합 요약](#1-통합-요약)
2. [QA — 기능 결함 분석](#2-qa--기능-결함-분석)
3. [코드 스멜 상세 진단](#3-코드-스멜-상세-진단)
4. [단계별 리팩토링 제안](#4-단계별-리팩토링-제안)
5. [회귀 테스트 및 QA 플랜](#5-회귀-테스트-및-qa-플랜)
6. [결론 및 우선순위 매트릭스](#6-결론-및-우선순위-매트릭스)

---

## 1. 통합 요약

### 1.1 QA 우선순위 (기능·명세)

| 우선순위 | 문제 | 영향 |
|----------|------|------|
| **P0** | 카테고리 필터가 `main` 키워드를 무시함 | "품질" 등 필터가 통계와 반대로 동작 |
| **P0** | 감성 분석 vs 필터가 **서로 다른 키워드 사전** 사용 | 화면 숫자와 필터 결과 불일치 |
| **P0** | **중립** 필터·통계 불일치 (`괜찮` 등) | 교육 과정에서도 "동작오류"로 지정된 항목 |
| **P1** | CSV가 `text` 컬럼이 아니라 **첫 번째 컬럼**만 사용 | README 명세 위반 |
| **P1** | 다운로드는 **마지막 필터 결과만**, 필터 없으면 빈 파일 | 사용자 기대와 다름 |
| **P1** | CSV 업로드 후 **분석 통계 미표시** | 기능 누락 |
| **P2** | 자동화 테스트 **0건** | 회귀 검증 불가 |
| **P2** | 로그는 콘솔만, UI에 level별 미표시 | `project_purpose.md` 미완 기능 |

### 1.2 코드 스멜 요약 (구조·유지보수)

| # | 코드 스멜 (Refactoring 명칭) | 주요 위치 | 위험도 | SOLID 위배 |
|---|------------------------------|-----------|--------|------------|
| 1 | **축약된 메서드명 (Obscured Intent / Poor Naming)** | `TextAnalyzer::sent`, `kw` / `Filters::fil` | 중 | SRP (의도 불명확으로 책임 경계 흐림) |
| 2 | **중복 키워드 등록 (Duplicated Data / Shotgun Surgery)** | `Constants.cpp`, `Filters::initFilterKeywords`, `UIComponents::CATS` | **높음** | OCP, DRY 위반 → 분석·필터 결과 불일치 |
| 3 | **의미 없는 Session (Fake Object / Lazy Class)** | `Session.h`, `Session.cpp`, `main.cpp` | **높음** | DIP, ISP (거짓 추상화) |
| 4 | **전역 상태 남용 (Global Data / Mutable Static)** | `main.cpp`, `TextAnalyzer`, `Session`, `Logger` | **높음** | SRP, 테스트 불가, 동시성 위험 |
| 5 | **FileHandler 스텁 (Dead Code / Speculative Generality)** | `FileHandler.h`, `/download` in `main.cpp` | 중 | SRP, DIP (저장 책임 미구현) |
| 6 | **God Module + Long Method** | `main.cpp` (`renderPage`, 라우트 핸들러) | 중 | SRP, OCP |
| 7 | **비공개 비즈니스 로직 중복 (Duplicate Logic)** | `TextAnalyzer`, `Filters` (`containsAny`, 감성 분류) | **높음** | SRP, OCP |

**위험도 기준**

- **높음:** 기능 버그·데이터 불일치·회귀 테스트 없이 수정 시 장애 가능성 큼
- **중:** 유지보수 비용 증가, 확장 시 수정 범위 확대
- **낮음:** 가독성·일관성 문제

### 1.3 스멜 ↔ QA 결함 매핑

| 코드 스멜 | 직접 연결되는 QA 결함 |
|-----------|----------------------|
| #2 중복 키워드 | P0 감성 통계≠필터, P0 중립 필터 오류 |
| #7 로직 중복 | P0 `main` 스킵과 별개로 감성·카테고리 규칙 불일치 |
| #4 전역 상태 | P1 다운로드 빈 파일, `fil_data` vs `Session` 분리 |
| #5 FileHandler 스텁 | P1 CSV 다운로드 형식·책임 분산 |
| #1, #3, #6 | 단독 사용자 장애보다 리팩토링·회귀 위험 |

---

## 2. QA — 기능 결함 분석

### 2.1 P0 — 반드시 잡아야 할 기능 결함

#### 2.1.1 카테고리 필터: 통계와 반대로 동작 (`main` 키워드 스킵)

**통계 (`TextAnalyzer::kw`)** 는 카테고리의 `"main"` 키워드만 사용합니다.

```21:42:src/cpp/TextAnalyzer.h
    std::map<std::string, int> kw(const std::vector<Feedback>& feedbacks) {
        // ...
                if (entry.second.count("main")) {
                    const auto& kws = entry.second.at("main");
                    if (containsAny(txt, kws)) {
                        res2[cat]++;
```

**필터 (`Filters::fil`)** 는 `"main"` 을 **의도적으로 건너뜁니다**.

```55:60:src/cpp/Filters.h
                    for (const auto& subEntry : catMap) {
                        if (subEntry.first == "main") continue;
                        if (containsAny(txt, subEntry.second)) {
                            finalFiltered.push_back(item);
```

**재현 시나리오**

1. 피드백 입력: `품질이 좋습니다`
2. `POST /analyze` → 키워드 분포에 **품질: 1** 표시
3. 감정=전체, 키워드=**품질** 로 `POST /filter`
4. **기대:** 1건 표시
5. **실제:** "필터링 결과가 없습니다" (서브 키워드에 `"품질"` 단독 문자열이 없음)

→ 코드 스멜 **#7** 과 결합된 **핵심 회귀 케이스** (테스트 K2, F2).

---

#### 2.1.2 감성 분석 vs 필터 — 이중 키워드 사전 (통계 ≠ 필터)

| 구분 | 데이터 소스 | 중립 처리 |
|------|-------------|-----------|
| 화면 통계 | `Constants::SENTIMENT_KEYWORDS` | 긍/부정 없으면 무조건 **중립** |
| 필터 | `Filters::S_KEYWORDS` (별도 `initFilterKeywords`) | 중립 **전용 키워드** + 긍정 우선 |

**재현 예시 A — `괜찮` 중복**

- `Filters.cpp` 긍정·중립 목록 **둘 다** `u8"괜찮"` 포함
- 필터: 긍정 먼저 검사 → **긍정**
- 통계: `Constants` 긍정에 `괜찮` 없음 → **중립**

입력 `괜찮아요` → 통계 중립 +1, 필터 "중립" 선택 시 **0건**.

**재현 예시 B — `늦다`**

- `Filters` 부정에만 `u8"늦다"` (`Constants` 부정에는 없음)
- 입력 `배송이 늦었어요` → 통계 **중립**, 필터 "부정"에는 **포함**

→ 코드 스멜 **#2** 의 직접적 사용자 영향. `project_purpose.md` 6.1절 **"필터 오류 수정(중립)"** 과제와 일치.

---

#### 2.1.3 긍정·부정 동시 포함 시 항상 긍정

```30:34:src/cpp/TextAnalyzer.h
            if (containsAny(txt, Constants::SENTIMENT_KEYWORDS[u8"긍정"])) {
                s = u8"긍정";
            } else if (containsAny(txt, Constants::SENTIMENT_KEYWORDS[u8"부정"])) {
                s = u8"부정";
```

예: `좋은데 나쁜 부분도 있음` → **긍정** (부정 무시). 규칙 버그이며 테스트 S4로 고정·문서화 필요.

---

### 2.2 P1 — 명세·UX와 어긋나는 문제

#### 2.2.1 CSV 업로드: `text` 컬럼 미사용

README: 필수 컬럼 `text`. 실제 코드는 **첫 번째 컬럼**만 사용.

```303:306:src/cpp/main.cpp
                        auto fields = parseCsvLine(line);
                        if (!fields.empty() && !fields[0].empty()) {
                            feedbacks.push_back(Feedback(fields[0]));
```

- `id,text` CSV → **id**가 본문으로 저장
- 헤더 행 **무조건** 스킵 → 헤더 없는 CSV면 첫 데이터 행 유실
- 업로드 후 감성/키워드 통계 **미표시**

#### 2.2.2 CSV 다운로드

```356:365:src/cpp/main.cpp
    svr.Get("/download", [](const httplib::Request&, httplib::Response& res) {
        // ...
        for (const auto& iter : fil_data) {
            csv << iter.getText() << "\n";
```

| 이슈 | 설명 |
|------|------|
| 데이터 소스 | `fil_data` 전역 — **성공한 필터 1회**만 반영 |
| 필터 없이 다운로드 | BOM + `text` 헤더만 있는 **빈 CSV** |
| 이스케이프 없음 | `,`, 줄바꿈 포함 시 **CSV 깨짐** |
| `FileHandler` | 스텁, `/download`에서 **미사용** |

#### 2.2.3 상태 3곳 분산

| 저장소 | 용도 |
|--------|------|
| `Session::currentFeedbacks` | 입력·업로드·필터 소스 |
| `fil_data` | 다운로드 전용 |
| `globalSent` / `globalKw` | 반환값 중복 캐시 (미사용) |

#### 2.2.4 UI/로그 미완

- 페이지에 level별 로그 미표시 (warning, error)
- `Logger` → `cout`/`cerr` 만
- `Filters::fil` → `std::cout` 부작용

---

### 2.3 P2 — QA 인프라·비기능

- `CMakeLists.txt` 테스트 타깃 없음, `tests/` 미존재
- 피드백 초기화 API 없음
- HTML `'` 미이스케이프, `0.0.0.0:8080` 바인딩
- 에러 메시지가 결과 섹션 **아래** 렌더
- 멀티스레드 확장 시 전역 static 레이스

---

## 3. 코드 스멜 상세 진단

### 3.1 축약된 메서드명 (`sent`, `kw`, `fil`)

```21:42:src/cpp/TextAnalyzer.h
    std::map<std::string, int> sent(const std::vector<Feedback>& feedbacks) {
    std::map<std::string, int> kw(const std::vector<Feedback>& feedbacks) {
```

```23:25:src/cpp/Filters.h
    std::vector<Feedback> fil(const std::vector<Feedback>& dataList,
                              const std::string& sFilter,
                              const std::string& kFilter) {
```

- 도메인 용어(감성 분포, 키워드 분포, 필터링)를 숨김
- `main.cpp` 274–275, 330, 333–334행 호출부만으로 의도 추론 어려움
- **SRP (간접):** HTTP 핸들러와 섞일 때 책임 경계 흐림

---

### 3.2 중복 키워드 등록 (Duplicated Data)

```7:21:src/cpp/Constants.cpp
    SENTIMENT_KEYWORDS[u8"긍정"] = {
        u8"좋아요", u8"만족", /* ... */
        u8"좋아요", u8"만족", u8"감사", u8"최고", /* 동일 블록 반복 */
    };
```

```5:20:src/cpp/Filters.cpp
void Filters::initFilterKeywords() {
    S_KEYWORDS[u8"긍정"] = { /* Constants와 유사하지만 목록 상이 */ };
    S_KEYWORDS[u8"중립"] = { /* TextAnalyzer는 중립 키워드 없음 */ };
}
```

```3:5:src/cpp/UIComponents.cpp
const std::vector<std::string> UIComponents::CATS = {
    u8"배송", u8"품질", u8"가격", u8"서비스", u8"사용성"
};
```

- 감성 키워드 **이중 정의** → [2.1.2](#212-감성-분석-vs-필터--이중-키워드-사전-통계--필터)
- 카테고리 **3곳 이상** 수정 필요 (Shotgun Surgery)
- **OCP / Single Source of Truth** 위반

---

### 3.3 의미 없는 Session (Fake Object)

```14:28:src/cpp/Session.h
    static void initSessionStateUgly() { /* already initialized */ }
    static std::vector<Feedback>& getOldDataFromSession(const std::string& key) {
        return currentFeedbacks;  // key 미사용
    }
```

- `internalData`, `filterOptions` — Dead Field
- HTTP 세션과 무관한 프로세스 전역 벡터
- **DIP, ISP, SRP** 위반

---

### 3.4 전역 상태 남용

```16:19:src/cpp/main.cpp
static std::vector<Feedback> fil_data;
static TextAnalyzer textAnalyzer;
static Filters filters;
static FileHandler fileHandler;
```

- `fileHandler` **미사용**
- → [2.2.2](#222-csv-다운로드), [2.2.3](#223-상태-3곳-분산)

---

### 3.5 FileHandler 스텁

```7:17:src/cpp/FileHandler.h
class FileHandler {
public:
    void saveResult(const std::vector<Feedback>& data) {
        std::cout << "saveResult" << data.size() << std::endl;
    }
};
```

실제 CSV는 `main` 인라인 — **Dead Code + 추상화 누락**.

---

### 3.6 God Module + Long Method

```80:211:src/cpp/main.cpp
static std::string renderPage(/* ... */) {
    // 130줄 이상 HTML/CSS/폼/결과 렌더링
```

`main.cpp` = 라우팅 + HTML + CSV 파싱 + 오케스트레이션.

---

### 3.7 비즈니스 로직 중복

- `containsAny` — `TextAnalyzer`, `Filters` 각각 구현
- 감성: `sent` vs `fil` 서로 다른 키워드 집합
- `Filters::fil` — `std::cout` 부작용 (68–70행)

---

## 4. 단계별 리팩토링 제안

각 Phase는 **이전 Phase 테스트 녹색** 후 진행.

### Phase 1 — 도메인 언어 복원 (저위험)

```cpp
// Before
sentimentResults = textAnalyzer.sent(feedbacks);
auto filtered = filters.fil(feedbacks, sentiment, keyword);

// After
sentimentResults = textAnalyzer.analyzeSentimentDistribution(feedbacks);
auto filtered = filters.filterBySentimentAndCategory(feedbacks, sentiment, keyword);
```

### Phase 2 — 키워드 단일화 (중위험, 효과 큼)

- `Filters::S_KEYWORDS` 제거 → `Constants` / `KeywordRegistry` 단일화
- `Constants::init()` 중복 push 제거
- `UIComponents::CATS` → `categoryNames()` 유도
- **QA:** F5 일관성, P0 중립·감성 불일치 해소

### Phase 3 — Session → FeedbackRepository

```cpp
class FeedbackRepository {
public:
    void add(const Feedback& fb);
    const std::vector<Feedback>& all() const;
    std::vector<Feedback> snapshot() const;
};
```

### Phase 4 — SentimentClassifier / CategoryMatcher 추출

- `containsAny` + 감성 판별 한 곳
- `Filters`는 classifier **주입**
- `globalSent`, `globalKw` 제거

### Phase 5 — FileHandler 실구현 또는 삭제

- **A:** `CsvFeedbackExporter` + `IFeedbackExporter`
- **B:** 미사용 시 `FileHandler` 삭제 (YAGNI)

### Phase 6 — 프레젠테이션 분리

`HtmlPageRenderer` + `PageViewModel` — `main.cpp`는 라우트만.

### Phase 7 — 전역/static 정리

| 제거 대상 | 대체 |
|-----------|------|
| `fil_data` | `FeedbackRepository::lastFilteredSnapshot()` |
| `globalSent`, `globalKw` | 반환값만 사용 |
| `Logger::debugMode` static | 주입 또는 환경 변수 |
| `main` static 서비스 | `FeedbackApp` 멤버 |

---

## 5. 회귀 테스트 및 QA 플랜

### 5.1 필수 자동화 테스트 (리팩토링 전)

#### SentimentClassifier / TextAnalyzer

| ID | 시나리오 | 입력 텍스트 | 기대 결과 |
|----|----------|-------------|-----------|
| S1 | 긍정 키워드 단일 | `배송이 빠르고 좋습니다` | `긍정`, +1 |
| S2 | 부정 | `품질이 나쁘고 실망입니다` | `부정` |
| S3 | 키워드 없음 | `그냥 무난한 하루` | `중립` |
| S4 | 긍·부정 혼재 | `좋은데 나쁜 부분도 있음` | `긍정` (if-else 순서 고정) |
| S5 | 빈 목록 | `[]` | `{긍정:0, 중립:0, 부정:0}` |
| S6 | 분포 합계 | N개 | `sum == N` |

#### CategoryMatcher / TextAnalyzer::kw

| ID | 시나리오 | 입력 | 기대 |
|----|----------|------|------|
| K1 | main 키워드 | `택배 배송이 늦음` | 배송 ≥ 1 |
| K2 | **main만** | `품질이 좋습니다` | 통계 품질 ≥ 1; 필터 품질은 **현재 0건 버그** |
| K3 | 복수 카테고리 | 배송+가격 동시 | 각 +1 여부 명시 |
| K4 | 미등록 카테고리 | — | 예외 없이 0 |

#### Filters

| ID | 시나리오 | 필터 | 기대 |
|----|----------|------|------|
| F1 | 감성=긍정 | 긍정+전체 | classifier 기준 긍정만 |
| F2 | 키워드=배송 | 전체+배송 | sub 키워드 (**main 스킵 주의**) |
| F3 | 복합 | 긍정+배송 | AND |
| F4 | 결과 없음 | 불가능 조합 | 빈 벡터 + warning |
| F5 | **일관성** | 동일 집합 | 통계 긍정 수 = 필터(긍정) 수 (Phase 2 후) |

#### Repository / HTTP

| ID | 시나리오 | 기대 |
|----|----------|------|
| R1 | add 후 size | 1씩 증가 |
| R2 | CSV `id,text` | 현재: 첫 컬럼만 (버그 문서화) |
| R3 | snapshot | 외부 수정이 내부에 반영 안 됨 |
| R4 | filter 후 download | 행 수 = 필터 결과 |
| H1 | POST `/analyze` | 200, 통계 |
| H2 | POST `/filter` (데이터 없음) | warning |
| H3 | GET `/download` (필터 후) | BOM, attachment, 행 수 일치 |
| H4 | POST `/upload` CSV | 헤더 제외 행 수만큼 증가 |

#### 리팩토링 게이트 (Definition of Done)

- [ ] Phase 2 후 **F5** 녹색
- [ ] `globalSent` / `fil_data` 제거 후 **R4, H3** 녹색
- [ ] Session 제거 후 **R1–R3** 녹색
- [ ] 한글 UI 메시지 golden string 1회

---

### 5.2 수동 QA 플랜

#### 스모크 (약 5분)

1. `http://localhost:8080` 접속
2. `품질이 좋습니다` → analyze → 품질 통계
3. 키워드=품질 필터 → **0건 = P0**
4. `괜찮아요` → analyze → 필터 중립 → **0건 = P0**

#### 감성·필터 일관성 (F5)

| 입력 | analyze 중립 | filter 중립 | filter 부정 |
|------|--------------|-------------|-------------|
| `괜찮아요` | +1 | 0건 | - |
| `배송이 늦었어요` | +1 | ? | 1건 |

#### CSV·다운로드

1. `id,text` 업로드 → id 저장 여부
2. 필터 없이 `/download` → 빈 파일
3. 필터 후 다운로드 → 행 수 일치
4. `배송, 좋아요` → CSV 깨짐

#### 경계

- 빈 텍스트 submit
- 피드백 0건 filter / download
- 긍·부정 혼합 1건

---

## 6. 결론 및 우선순위 매트릭스

### 6.1 의도적 스멜 vs 진짜 QA 타깃

| 구분 | 예시 | 대응 |
|------|------|------|
| 의도적 스멜 | `fil`/`sent`/`kw`, God `main`, Fake `Session` | 리팩토링 대상 |
| **진짜 QA 타깃** | `main` 스킵, 이중 감성 사전, `fil_data`, CSV 컬럼 | **P0/P1 테스트 필수** |
| 교육 로드맵 | 중립 필터, 로그 UI, 테스트 90% | `project_purpose.md` |

### 6.2 통합 작업 우선순위

| 순위 | 작업 | QA·리팩토링 이유 |
|------|------|------------------|
| 1 | `Filters::fil` — `main` 스킵 제거 또는 `kw` 와 동일 규칙 | P0 품질 필터 등 |
| 2 | 키워드 단일화 + `SentimentClassifier` | P0 F5, Phase 2 |
| 3 | CSV `text` 컬럼, 다운로드 소스 통일 | P1 R2, H3 |
| 4 | Session → Repository | 전역 상태 축소 |
| 5 | Catch2 타깃 + S1–H4 | 회귀 방지 |
| 6 | FileHandler 구현 또는 삭제 | Dead code |
| 7 | HTML 분리 | SRP |

---

## 관련 문서

- [../project_purpose.md](../project_purpose.md) — 프로젝트 목적·교육 시나리오
- [../README.md](../README.md) — 빌드·실행·CSV 명세
- `docs/code_smell.md` — **본 문서로 통합됨** (참고용 원본)

---

*본 문서는 `src/cpp` 정적 분석·QA 시나리오 기준이며, 코드 변경 시 해당 섹션을 갱신하는 것을 권장합니다.*
