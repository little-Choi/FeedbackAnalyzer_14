# Feedback Analyzer — 코드 스멜 분석 및 리팩토링 제안

**분석 대상:** `src/cpp/`  
**분석 일자:** 2026-05-22  
**관점:** Clean Code, Refactoring, SOLID

---

## 1. 코드 스멜 분석 요약

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
- **낮음:** 가독성·일관성 문제 (본 보고서에서는 상위 7건만 상세 기술)

---

## 2. 스멜별 상세 진단 및 이유

### 2.1 축약된 메서드명 (`sent`, `kw`, `fil`)

**원인 코드**

```21:42:src/cpp/TextAnalyzer.h
    std::map<std::string, int> sent(const std::vector<Feedback>& feedbacks) {
        // ...
    }

    std::map<std::string, int> kw(const std::vector<Feedback>& feedbacks) {
```

```23:25:src/cpp/Filters.h
    std::vector<Feedback> fil(const std::vector<Feedback>& dataList,
                              const std::string& sFilter,
                              const std::string& kFilter) {
```

**진단**

- `sent` / `kw` / `fil`은 도메인 용어(감성 분포, 키워드 분포, 필터링)를 숨깁니다.
- 호출부(`main.cpp` 274–275, 330, 333–334행)만 봐서는 반환 타입·부작용을 추론하기 어렵습니다.

**SOLID 위배**

- **SRP (간접):** 이름이 책임을 드러내지 않아, 분석 로직이 HTTP 핸들러와 한 파일에 섞일 때 책임 경계가 더 흐려집니다.
- **가독성 = 유지보수성:** Clean Code의 “의도를 드러내는 이름” 원칙 위반.

---

### 2.2 중복 키워드 등록 (Duplicated Data)

**원인 코드**

```7:21:src/cpp/Constants.cpp
    SENTIMENT_KEYWORDS[u8"긍정"] = {
        u8"좋아요", u8"만족", /* ... */
        u8"좋아요", u8"만족", u8"감사", u8"최고", /* 동일 블록 반복 */
    };
    SENTIMENT_KEYWORDS[u8"부정"] = {
        /* ... */
        u8"나쁘", u8"불만", /* ... 동일 항목 재등록 */
    };
```

```5:20:src/cpp/Filters.cpp
void Filters::initFilterKeywords() {
    S_KEYWORDS[u8"긍정"] = { u8"좋아요", u8"만족", /* Constants와 유사하지만 목록 상이 */ };
    S_KEYWORDS[u8"부정"] = { /* ... */ };
    S_KEYWORDS[u8"중립"] = { /* TextAnalyzer는 중립 키워드 없음 */ };
}
```

```3:5:src/cpp/UIComponents.cpp
const std::vector<std::string> UIComponents::CATS = {
    u8"배송", u8"품질", u8"가격", u8"서비스", u8"사용성"
};
```

**진단**

- 감성 키워드가 `Constants::SENTIMENT_KEYWORDS`와 `Filters::S_KEYWORDS` **두 곳**에 따로 정의됩니다.
- `Constants.cpp` 내부에서도 동일 문자열이 **두 번** push됩니다 (실질적 중복 데이터).
- 카테고리는 `Constants::CATEGORY_KEYWORDS`와 `UIComponents::CATS`에 **이중 등록** — 새 카테고리 추가 시 3곳 이상 수정 필요 (Shotgun Surgery).

**SOLID 위배**

- **OCP:** 키워드·카테고리 추가 시 기존 클래스를 계속 수정해야 하며, 확장이 “닫혀” 있지 않습니다.
- **단일 진실 공급원(Single Source of Truth) 부재:** `TextAnalyzer::sent`와 `Filters::fil`이 서로 다른 키워드 집합을 쓰면, **화면 통계와 필터 결과가 어긋날 수 있습니다** (예: `Constants`에만 있는 `u8"훌륭"` vs `Filters`에만 있는 `u8"뛰어납니다"`).

---

### 2.3 의미 없는 Session (Fake Object / Lazy Class)

**원인 코드**

```14:28:src/cpp/Session.h
    static void initSessionStateUgly() {
        // already initialized as static
    }

    static std::vector<Feedback>& getOldDataFromSession(const std::string& key) {
        return currentFeedbacks;  // key 미사용
    }

    static std::vector<Feedback>& getCurrentFeedbacks() {
        return currentFeedbacks;
    }
```

```241:242:src/cpp/main.cpp
        Session::initSessionStateUgly();
        auto& feedbacks = Session::getOldDataFromSession("current_feedbacks");
```

**진단**

- `key` 파라미터는 무시됩니다 → **거짓 API**.
- `internalData`, `filterOptions`는 선언만 있고 사용되지 않습니다 (Dead Field).
- HTTP 세션/쿠키와 무관한 **프로세스 전역 벡터**를 Session이라 부릅니다.

**SOLID 위배**

- **DIP:** 상위 모듈(`main`)이 구체적 전역 저장소에 직접 의존.
- **ISP:** `getOldDataFromSession(key)`는 키 기반 접근을 암시하지만 제공하지 않음.
- **SRP:** “세션 관리” 책임이 없고 단순 전역 변수 래퍼입니다.

---

### 2.4 전역 상태 남용 (Global Data)

**원인 코드**

```16:19:src/cpp/main.cpp
static std::vector<Feedback> fil_data;
static TextAnalyzer textAnalyzer;
static Filters filters;
static FileHandler fileHandler;
```

```9:11:src/cpp/TextAnalyzer.h
    static std::map<std::string, int> globalSent;
    static std::map<std::string, int> globalKw;
```

```37:38:src/cpp/TextAnalyzer.cpp
        globalSent = res;
        return res;
```

```332:332:src/cpp/main.cpp
                    fil_data = filtered;
```

**진단**

- 피드백 저장: `Session::currentFeedbacks`
- 다운로드용 스냅샷: `fil_data` (별도 전역)
- 분석 캐시: `globalSent`, `globalKw` (반환값과 중복 저장)
- `fileHandler`는 선언만 되고 **미사용**

**SOLID 위배**

- **SRP:** 상태 소유 주체가 분산되어 “누가 최신 데이터인가”가 불명확합니다.
- **테스트:** 단위 테스트 시 상태 초기화·격리가 어렵습니다.
- **동시성:** `httplib` 멀티스레드 확장 시 데이터 레이스 위험.

---

### 2.5 FileHandler 스텁 (Dead Code / Incomplete Abstraction)

**원인 코드**

```7:17:src/cpp/FileHandler.h
class FileHandler {
public:
    void saveResult(const std::vector<Feedback>& data) {
        std::cout << "saveResult" << data.size() << std::endl;
        // stdout only — no file I/O
    }
    void save(const std::vector<Feedback>& data) {
        saveResult(data);
    }
};
```

```356:365:src/cpp/main.cpp
    svr.Get("/download", [](const httplib::Request&, httplib::Response& res) {
        std::ostringstream csv;
        csv << "\xEF\xBB\xBF";
        csv << "text\n";
        for (const auto& iter : fil_data) {
            csv << iter.getText() << "\n";
        }
        // FileHandler 미사용
    });
```

**진단**

- `FileHandler`는 이름과 달리 **콘솔 출력 스텁**입니다.
- 실제 CSV 생성·다운로드는 `main`의 람다에 인라인 구현 → **추상화 누락 + Dead Code**.

**SOLID 위배**

- **SRP:** 영속화 책임이 `main`과 `FileHandler`에 분산·공백.
- **DIP:** `IFeedbackExporter` 같은 인터페이스 없이 상위가 구현 세부(CSV, BOM)에 묶임.

---

### 2.6 God Module + Long Method (`main.cpp`)

**원인 코드**

```80:211:src/cpp/main.cpp
static std::string renderPage(/* ... */) {
    std::ostringstream html;
    html << R"(<!DOCTYPE html>
    // 130줄 이상 인라인 HTML/CSS/폼/결과 렌더링
```

**진단**

- `main.cpp`가 HTTP 라우팅, 폼 파싱, HTML 렌더링, CSV 파싱, 오케스트레이션을 모두 담당합니다.
- `renderPage`는 **Long Method** + **Mixed Levels of Abstraction**.

**SOLID 위배**

- **SRP:** 진입점이 너무 많은 이유를 알고 있습니다.
- **OCP:** UI 문구·스타일 변경도 `main.cpp` 재컴파일 필요.

---

### 2.7 비공개 비즈니스 로직 중복 (Duplicate Logic)

**원인 코드**

```13:18:src/cpp/TextAnalyzer.h
    static bool containsAny(const std::string& text, const std::vector<std::string>& keywords) {
```

```13:18:src/cpp/Filters.h
    static bool containsAny(const std::string& text, const std::vector<std::string>& keywords) {
```

감성 분류 로직:

- `TextAnalyzer::sent` — `Constants::SENTIMENT_KEYWORDS` 사용 (27–35행)
- `Filters::fil` — `S_KEYWORDS` 사용 (28–39행), 추가로 `std::cout` 부작용 (68–70행)

**SOLID 위배**

- **SRP:** 필터가 “선별”과 “분류 알고리즘”을 동시에 소유.
- **OCP:** 분류 규칙 변경 시 두 클래스를 함께 수정해야 함.
- **DRY 위반:** 동일 도메인 규칙이 두 구현으로 갈라짐.

---

## 3. 단계별 리팩토링 제안

리팩토링은 **아래 순서**를 권장합니다. 각 단계는 이전 단계의 테스트가 녹색일 때만 진행합니다.

### Phase 1 — 도메인 언어 복원 (저위험)

**목표:** 축약 메서드명 제거, 호출부 가독성 확보.

**Before**

```cpp
sentimentResults = textAnalyzer.sent(feedbacks);
keywordResults = textAnalyzer.kw(feedbacks);
auto filtered = filters.fil(feedbacks, sentiment, keyword);
```

**After**

```cpp
sentimentResults = textAnalyzer.analyzeSentimentDistribution(feedbacks);
keywordResults = textAnalyzer.analyzeKeywordDistribution(feedbacks);
auto filtered = filters.filterBySentimentAndCategory(feedbacks, sentiment, keyword);
```

---

### Phase 2 — 키워드 단일화 (중위험, 효과 큼)

**목표:** `Filters::S_KEYWORDS` 제거, `Constants`를 유일한 레지스트리로.

**Before**

```cpp
// Filters.cpp — 별도 init
void Filters::initFilterKeywords() {
    S_KEYWORDS[u8"긍정"] = { u8"좋아요", /* ... */ };
}
// TextAnalyzer — Constants 사용
containsAny(txt, Constants::SENTIMENT_KEYWORDS[u8"긍정"]);
```

**After**

```cpp
class KeywordRegistry {
public:
    static const std::map<std::string, std::vector<std::string>>& sentimentKeywords();
    static const std::map<std::string, std::map<std::string, std::vector<std::string>>>& categoryKeywords();
    static std::vector<std::string> categoryNames(); // CATEGORY_KEYWORDS 키에서 유도
};

// Filters::fil 내부
containsAny(txt, KeywordRegistry::sentimentKeywords().at(u8"긍정"));
```

**추가:** `Constants::init()`에서 중복 push 제거, `UIComponents::CATS` 삭제 후 `categoryNames()` 사용.

---

### Phase 3 — Session 제거 → 명시적 저장소 (중위험)

**목표:** Fake Session을 `FeedbackRepository`로 교체.

**Before**

```cpp
auto& feedbacks = Session::getCurrentFeedbacks();
feedbacks.push_back(Feedback(text));
```

**After**

```cpp
class FeedbackRepository {
public:
    void add(const Feedback& fb);
    const std::vector<Feedback>& all() const;
    void replaceAll(const std::vector<Feedback>& items);
    std::vector<Feedback> snapshot() const; // 다운로드용
};

// main 또는 ApplicationContext가 소유 (static이 아닌 멤버)
class FeedbackApp {
    FeedbackRepository repo_;
    TextAnalyzer analyzer_;
    Filters filters_;
public:
    void handleAnalyze(const FormParams& params);
};
```

**SOLID 개선:** DIP — HTTP 핸들러는 `FeedbackApp`에만 의존.

---

### Phase 4 — 감성/키워드 분류기 추출 (중위험)

**목표:** `containsAny` + 감성 판별을 한 곳으로.

**Before** (TextAnalyzer + Filters 각각 구현)

**After**

```cpp
class SentimentClassifier {
public:
    std::string classify(const std::string& text) const;
};
class CategoryMatcher {
public:
    bool matchesCategory(const std::string& text, const std::string& category) const;
};

// TextAnalyzer
std::map<std::string, int> TextAnalyzer::analyzeSentimentDistribution(
    const std::vector<Feedback>& feedbacks) {
    std::map<std::string, int> res{{u8"긍정",0},{u8"중립",0},{u8"부정",0}};
    for (const auto& f : feedbacks)
        res[classifier_.classify(f.getText())]++;
    return res;  // globalSent 제거
}
```

`Filters`는 `SentimentClassifier`를 **주입**받아 동일 규칙으로 필터링.

---

### Phase 5 — FileHandler 실구현 또는 삭제 (저~중위험)

**선택 A — 구현**

```cpp
class CsvFeedbackExporter : public IFeedbackExporter {
public:
    std::string exportWithBom(const std::vector<Feedback>& data) const override;
};
// GET /download
res.set_content(exporter.exportWithBom(repo.snapshot()), "text/csv; charset=UTF-8");
```

**선택 B — 미사용이면 삭제:** `FileHandler` 및 `main`의 `fileHandler` 전역 제거 (YAGNI).

---

### Phase 6 — 프레젠테이션 분리 (중위험)

**목표:** `renderPage` → `HtmlPageRenderer` / 템플릿 파일.

**After (구조 예시)**

```cpp
class HtmlPageRenderer {
public:
    std::string render(const PageViewModel& vm) const;
};

struct PageViewModel {
    std::string success, warning, error;
    std::map<std::string, int> sentimentResults, keywordResults;
    std::vector<Feedback> feedbacks;
};
```

`main.cpp`는 라우트 등록과 `FeedbackApp` 호출만 남깁니다.

---

### Phase 7 — 전역/static 정리 (마지막)

| 제거 대상 | 대체 |
|-----------|------|
| `fil_data` | `FeedbackRepository::lastFilteredSnapshot()` |
| `globalSent`, `globalKw` | 반환값만 사용 (캐시 불필요 시 삭제) |
| `Logger::debugMode` static | 생성자 주입 또는 환경 변수 |
| `main` 내 static 서비스 | `FeedbackApp` 멤버 |

---

## 4. 회귀 버그 방지를 위한 필수 테스트 케이스 정의

현재 프로젝트에 자동화 테스트가 없으므로, **리팩토링 착수 전** 아래 시나리오를 Google Test / Catch2 등으로 최소 구현하는 것을 권장합니다.

### 4.1 `SentimentClassifier` / `TextAnalyzer`

| ID | 시나리오 | 입력 텍스트 | 기대 결과 |
|----|----------|-------------|-----------|
| S1 | 긍정 키워드 단일 | `"배송이 빠르고 좋습니다"` | 분류 `"긍정"`, 분포 카운트 +1 |
| S2 | 부정 우선 (부정 키워드 포함) | `"품질이 나쁘고 실망입니다"` | `"부정"` |
| S3 | 키워드 없음 | `"그냥 무난한 하루"` | `"중립"` |
| S4 | 긍·부정 혼재 | `"좋은데 나쁜 부분도 있음"` | **현재 규칙 고정:** if-else 순서상 `"긍정"` (문서화 후 변경 시 테스트 갱신) |
| S5 | 빈 피드백 목록 | `[]` | `{긍정:0, 중립:0, 부정:0}` |
| S6 | 분포 합계 | N개 피드백 | `sum(sentimentResults) == N` |

### 4.2 `CategoryMatcher` / `TextAnalyzer::kw`

| ID | 시나리오 | 입력 | 기대 |
|----|----------|------|------|
| K1 | main 키워드 매칭 | `"택배 배송이 늦음"` | `"배송"` 카운트 ≥ 1 |
| K2 | 서브카테고리만 (main 없음) | main 없이 sub만 있는 경우 | **현재 구현:** `kw`는 `"main"`만 사용 → 0 유지 (동작 고정) |
| K3 | 복수 카테고리 | 배송+가격 동시 언급 | 해당 카테고리 각각 +1 가능 여부 명시 |
| K4 | 미등록 카테고리 | — | 예외 없이 0 |

### 4.3 `Filters::filterBy...`

| ID | 시나리오 | 필터 | 기대 |
|----|----------|------|------|
| F1 | 감성=긍정 | `sentiment="긍정", keyword="전체"` | 모든 항목이 classifier 기준 긍정 |
| F2 | 키워드=배송 | `sentiment="전체", keyword="배송"` | 텍스트에 배송 sub 키워드 포함 |
| F3 | 복합 필터 | 긍정 + 배송 | AND 조건 충족만 |
| F4 | 결과 없음 | 존재하지 않는 조합 | 빈 벡터, UI warning 경로와 일치 |
| F5 | **분석·필터 일관성** | 동일 피드백 집합 | `TextAnalyzer` 긍정 개수 ≥ `filter(긍정)` 개수 (단일 classifier 사용 후 **동일**해야 함) |

### 4.4 `FeedbackRepository` / 세션 대체

| ID | 시나리오 | 기대 |
|----|----------|------|
| R1 | add 후 size | 1씩 증가 |
| R2 | CSV 업로드 시뮬레이션 | 헤더 스킵, 첫 컬럼만 Feedback 생성 |
| R3 | snapshot 불변성 | `snapshot()` 수정이 내부 저장소를 바꾸지 않음 |
| R4 | filter 후 download 데이터 | 마지막 필터 결과만 exporter에 전달 |

### 4.5 HTTP 통합 (선택, E2E)

| ID | 시나리오 | 기대 HTTP |
|----|----------|-----------|
| H1 | POST `/analyze` text=단일 | 200, 본문에 성공 메시지·통계 |
| H2 | POST `/filter` without prior data | warning `"분석할 피드백이 없습니다"` |
| H3 | GET `/download` after filter | `Content-Disposition` attachment, UTF-8 BOM, 행 수 = 필터 결과 수 |
| H4 | POST `/upload` CSV | 헤더 제외 행 수만큼 피드백 증가 |

### 4.6 리팩토링 게이트 (Definition of Done)

- [ ] Phase 2 완료 후 **F5 일관성 테스트** 녹색  
- [ ] `globalSent` / `fil_data` 제거 후 **R4, H3** 녹색  
- [ ] Session 삭제 후 **R1–R3** 녹색  
- [ ] 기존 수동 시나리오(한글 UI 메시지) 스냅샷 또는 golden string 비교 1회

---

## 부록: 우선순위 매트릭스

| 순위 | 작업 | 이유 |
|------|------|------|
| 1 | 키워드 단일화 + 중복 제거 | 데이터 불일치 버그 직접 예방 |
| 2 | `SentimentClassifier` 추출 + F5 테스트 | 필터/분석 동기화 |
| 3 | Session → Repository | 전역 상태 축소 |
| 4 | FileHandler 구현 또는 삭제 | Dead code 제거 |
| 5 | HTML 분리 | SRP, UI 변경 용이 |

---

*본 문서는 `src/cpp` 정적 분석 기준이며, 리팩토링 적용 시 각 Phase별 커밋과 테스트 녹색 상태를 유지하는 것을 권장합니다.*
