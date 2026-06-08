# Invalid Missing Fields Dataset

## Purpose

This dataset verifies that the parser or input-loading flow handles malformed course input safely.

The courses file intentionally contains a course entry with a missing required field.

## Invalid Case

The course entry is missing the instructor name.

Expected course format:

text:
$$$$
Course Name
Course Number
Instructor Name
ProgramId,Year,Semester,Requirement
EvaluationType

Actual invalid structure:

text:
$$$$
Calculus 1
83112
83101,1,FALL,Obligatory
Exam

## Expected Result

The system should not crash.

The parser or input-loading flow should reject the malformed input or report a clear error.

No valid schedule should be generated from this dataset.

## Expected Status

Fail expected / handled error expected
