# User 프롬프트만 — 001-20260522-spec-branch-report-agent

- **보고서 번호**: 001
- **저장일**: 2026-05-22

---

## User Prompt 1

spec branch 만들어줘.

## User Prompt 2

spec branch 다시 제거해줘.

## User Prompt 3

보고서 작성 및 프롬프트 저장, 깃 허브를 관리하는 Agent를 .cursor/agent 에 생성해줘.
"보고서 작성 및 저장 Agent 실행해줘" 명령을 받으면 아래 작업을 순차적으로 진행하는 Agent 생성해줘.
[1] 작업한 내용에 대한 보고서 작성 : Report 폴더에 파일번호가 들어간 .md 파일로 작성
[2] 작업한 모든 대화내용 저장 : 현재까지의 프롬프트를 대화형 프롬프트로 Prompting 폴더에 보고서 파일명에 -Prompt를 붙여서 저장
[3] 대화내용 중 User 프롬프트 저장 : Prompting_user 폴더에 User 프롬프트만 추출하여 보고서 파일명에 -User를 붙여서 저장
[4] 깃허브에 모든 작업내용 commit에 push까지 진행하기.

## User Prompt 4

보고서 작성 및 저장 Agent 실행해줘
