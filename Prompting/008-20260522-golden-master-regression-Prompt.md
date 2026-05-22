# 대화 프롬프트 기록 — 008-20260522-golden-master-regression

- **보고서 번호**: 008
- **저장일**: 2026-05-22

---

## Turn 1

**User**

@README.md @docs/qa_analysis.md @docs/test_plan.md @project_purpose.md

[P] 회귀 테스트(Approval/Golden Master) 설계 전문가입니다.
[C] C++17, Google Test, CMake
[T] TexttestFixture 출력 기반 Golden Master를 설계/구현해줘.
    1) expected 출력 파일 생성/보관 전략
    2) catch2에서 파일 비교 테스트 구현
    3) CMake/ctest에 통합하는 방법
    4) CI에서 자동 실행 구성
[F] 테스트 코드 + CMake 수정안 + 실행 방법

**Assistant**

Golden Master 회귀 테스트를 설계·구현했습니다.

**1) expected 보관:** `tests/expected/{domain,csv,http}/`, Git 커밋, `GOLDEN_EXPECTED_DIR` 매크로, CRLF 정규화, `FEEDBACK_ANALYZER_UPDATE_GOLDEN=1`로만 갱신.

**2) Catch2:** `GoldenMaster.h`의 `assertMatchesGolden`, `GoldenSerialize.h`, `tests/golden/test_golden_outputs.cpp` 9 cases `[golden][regression]`.

**3) CMake/ctest:** `tests/CMakeLists.txt`에 소스·타깃 `update_golden`, `ctest -R "GM-"` 또는 `exe "[golden]"`.

**4) CI:** `.github/workflows/tests.yml` regression job에 golden 단계 추가.

**검증:** golden 9건 + 전체 76 cases Green. 문서 `docs/golden_master.md`, 스크립트 `scripts/update_golden.ps1`.

---

## Turn 2

**User**

(System) 빌드 task 완료 알림 — `feedback_analyzer_tests` 빌드 성공.

**Assistant**

빌드 완료 안내. Golden Master 9건·전체 테스트 통과 상태, baseline 없으면 `update_golden` 실행 안내.

---

## Turn 3

**User**

'보고서 작성 및 저장 Agent' 실행해줘

**Assistant**

Report & GitHub Manager 워크플로 실행 — 보고서 008, Prompting/Prompting_user 저장, Git commit·push.
