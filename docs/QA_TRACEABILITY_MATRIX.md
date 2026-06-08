# QA Traceability Matrix - Version 2.0

This document maps SRS requirements to planned QA coverage.

| SRS Area | Requirement Summary | Test Type | Test Location / Task | Status | Notes |
|---|---|---|---|---|---|
| Input Screen | Load courses and exam-period files | Integration + Manual | TBD | Planned | Covers valid and invalid file loading. |
| Input Screen | Replace existing data with new files | Integration + Manual | TBD | Planned | Needs controlled datasets. |
| Input Screen | Append additional data without deleting existing data | Integration + Manual | TBD | Planned | Needs duplicate-handling expectations. |
| Program Selection | Select up to five study programs | Unit + Manual | TBD | Planned | Verify sixth selection is blocked or handled clearly. |
| Program Selection | Display selected program details | Manual | TBD | Planned | Includes program name and ID. |
| Program Selection | Display courses by year and semester | Manual | TBD | Planned | Depends on implemented UI support. |
| Calendar Editing | Display exam-period calendar | Layout + Manual | TBD | Planned | Calendar should show active and excluded days. |
| Calendar Editing | Exclude and restore individual days | Unit + Manual | TBD | Planned | Validates excluded-day behavior. |
| Calendar Editing | Shift exam-period start/end dates | Integration + Manual | TBD | Planned | Depends on implemented period editor. |
| Scheduler | Generate possible exam schedules | Unit + Integration | TBD | Planned | Core scheduling flow. |
| Scheduler | Prevent invalid overlaps | Unit + Integration | TBD | Planned | Requires dedicated overlap datasets. |
| Scheduler | Handle no-solution cases | Integration | TBD | Planned | Requires no-solution dataset. |
| Output Screen | Display one generated schedule at a time | Layout + Manual | TBD | Planned | Output screen validation. |
| Output Screen | Navigate between generated schedules | Manual | TBD | Planned | Verify previous/next behavior and counter. |
| Output Screen | Display exam details in schedule | Layout + Manual | TBD | Planned | Course number, name, date, instructor where available. |
| Export | Save selected generated schedule to readable file | Integration + Manual | TBD | Planned | Should export selected generated schedule, not calendar-period state. |
| Export | Decide whether exported file must be parser-compatible | Documentation | TBD | Planned | Human-readable and parser-compatible are different requirements. |
| Performance | UI remains responsive during scheduling | Manual | TBD | Planned | Especially during long scheduling generation. |
| Regression | Validate full user flow before submission | Manual | TBD | Planned | Final release validation. |