# 📘 Exam Scheduling System – Version 1.0

## ✨ Overview
The **Exam Scheduling System** is a software project designed to generate **all valid exam scheduling options** for selected engineering study programs.

As the number of courses and academic programs grows, scheduling exams manually becomes more difficult and may lead to conflicts or inefficient solutions. This system helps solve that problem by automatically generating all possible valid exam systems based on structured input files and predefined scheduling rules.

**Version 1.0** focuses on the core functionality of the system:
- reading input data from files,
- identifying relevant courses that require exams,
- generating all valid exam schedules,
- preventing critical conflicts,
- and producing a clear, human-readable output file.

This version is implemented as a **file-based application** and follows **Object-Oriented Programming (OOP)** principles.

---

## 🎯 Project Objectives
The system is intended to:

- Read academic and scheduling data from input files
- Allow the user to choose up to **5 study programs**
- Generate **all possible valid exam schedules** for the selected programs
- Prevent critical exam conflicts according to the official project rules
- Output all valid exam systems in a clear and sorted format
- Provide a modular and extensible architecture for future versions

---

## 🧩 Scope of Version 1.0
Version 1.0 includes:

- Input handling through files only
- Parsing course data, exam periods, and selected programs
- Filtering courses whose evaluation method is `Exam`
- Generating all valid scheduling combinations
- Detecting conflicts according to the defined rules
- Writing all valid exam systems to a readable output file

Version 1.0 does **not** include:

- Graphical user interface
- Advanced filtering and sorting by user preferences
- Automatic schedule validation tools
- Advanced recommendation or optimization features

These features are planned for future versions of the system.

---

## ⚙️ Functional Requirements

### 📌 Program Selection
The user may select up to **5 study programs** from the predefined list of supported engineering programs.

### 📚 Course Filtering
The system considers only courses whose evaluation type is **`Exam`**.

### 🗓️ Schedule Generation
The system generates **all possible valid exam systems** for the selected study programs.

### 🚫 Conflict Rules
A generated exam schedule is considered valid if it satisfies the following rule:

- Two exams that belong to the **same study program** and the **same academic year** may **not** be scheduled on the same date,
- unless **both courses are elective**.

In **Version 1.0**:
- conflicts are checked **by date only**
- exam hours are **not** taken into account
- conflicts between two elective courses are allowed
- conflicts involving repeated courses may still exist

---

## 📥 Input
The system receives **three input files**.

### 1️⃣ Courses File
Contains all available courses in the system, including:

- Course name
- Course number
- Instructor name
- Study program(s)
- Academic year
- Semester
- Requirement type (`Obligatory` / `Elective`)
- Evaluation type (`Exam` / `Project` / `Attendance`)

### 2️⃣ Exam Periods File
Contains exam scheduling periods, including:

- Semester
- Exam period (`Aleph`, `Bet`, `Gimel`)
- Start date
- End date
- Excluded dates or excluded date ranges  
  for example: holidays, Saturdays, unavailable dates

### 3️⃣ Selected Programs File
Contains the IDs of the study programs selected by the user.

Example:
```text
83101, 83102, 83108
```

---

## 📤 Output
The output of the system is a list of **all valid exam systems**.

Each generated exam system includes:

- all exams included in that system
- for each exam:
  - course name
  - course number
  - instructor name
  - exam date
  - semester
  - exam period

The output is:

- clearly formatted
- human-readable
- sorted by exam date
- separated by semester
- separated by exam period

---

## 🏫 Supported Study Programs
The following study programs are supported in Version 1.0:

- `83101` – Computer Engineering
- `83102` – Electrical Engineering
- `83104` – Industrial Engineering and Information Systems
- `83107` – Data Engineering
- `83108` – Software Engineering
- `83109` – Materials Engineering
- `83105` – Computer Hardware Track – Computer Engineering
- `83182` – Quantum Engineering Track – Electrical Engineering
- `83103` – Neuroengineering Track – Electrical Engineering
- `83115` – Biomedical Engineering Track – Electrical Engineering

---

## 🏗️ System Design
The system is designed according to **Object-Oriented Programming (OOP)** principles.

The architecture separates responsibilities into distinct components such as:

- data representation
- file parsing
- date generation
- conflict validation
- schedule generation
- output formatting

This modular design improves readability, maintainability, testability, and support for future extensions.

### Suggested Core Classes
- `Course`
- `CourseOffering`
- `ExamPeriod`
- `ExamAssignment`
- `ExamSystem`
- `InputParser`
- `DateGenerator`
- `ConflictChecker`
- `ScheduleGenerator`
- `OutputWriter`

---

## 🧠 Scheduling Strategy
The core engine of the system is responsible for generating **all valid schedule combinations**.

A suitable approach for Version 1.0 is:

1. Read and parse all input files
2. Filter only relevant courses with `Exam` evaluation
3. Generate all legal dates from exam periods while excluding blocked dates
4. Assign dates to courses
5. Check for conflicts during assignment
6. Save every fully valid schedule as a separate exam system

A common implementation strategy for this process is a **backtracking / search-based algorithm**, with pruning of invalid branches when conflicts are detected.

---

## 🚀 Performance Requirement
According to the project requirements, the generation of the full output should take **no more than 30 seconds**.

For this reason, the scheduling logic should be implemented efficiently and should avoid unnecessary repeated checks whenever possible.

---

## 🗂️ Project Structure
A recommended project structure is shown below:

```text
project-root/
│
├── data/
│   ├── courses.txt
│   ├── exam_periods.txt
│   └── selected_programs.txt
│
├── src/
│   ├── model/
│   ├── parser/
│   ├── scheduler/
│   ├── validation/
│   └── output/
│
├── tests/
│   ├── unit/
│   └── integration/
│
├── docs/
│   ├── design_document.md
│   ├── test_plan.md
│   └── code_review_summary.md
│
├── output/
│   └── generated_schedules.txt
│
├── main.py
└── README.md
```

> If the project is implemented in C++, replace `main.py` with the relevant C++ entry file and build files.

---

## ▶️ Build and Run

### Python
```bash
python main.py data/courses.txt data/exam_periods.txt data/selected_programs.txt
```

### C++
```bash
g++ -std=c++17 -o exam_scheduler main.cpp
./exam_scheduler data/courses.txt data/exam_periods.txt data/selected_programs.txt
```

> Use the command that matches your implementation language.

---

## ✅ Testing
The project should include the following types of tests:

### Unit Tests
- Course parsing
- Exam period parsing
- Date generation
- Conflict checking
- Output formatting

### Integration Tests
- Full end-to-end execution using example input files
- Verification that all required exam courses are scheduled
- Verification that no invalid schedule is included in the output

### Edge Case Tests
- No selected programs
- No exam courses
- All dates blocked
- Invalid input format
- Maximum number of selected programs
- Multiple overlapping program-year combinations

### Performance Tests
- Large input scenarios
- Runtime validation against the 30-second requirement

---

## 🔄 Development Process
The project is developed according to the official requirements using:

- **Git** for version control
- **JIRA** for task and workflow management
- **Agile methodology** for iterative development
- **Code Review** as part of the development and quality assurance process

---

## 📝 Documentation
The project documentation should include:

- Software design document
- Test plan
- Implementation documentation
- Code review summary
- Test execution results

In addition, the code itself should contain internal documentation that explains the purpose of key classes, methods, and variables.

---

## 🔮 Future Versions
According to the requirements, future versions of the system are expected to include:

### Version 2.0
- Advanced interface capabilities

### Version 3.0
- Automatic exam schedule validation tools
- Sorting and filtering of results
- Better adaptation to user preferences

### Version 4.0
- Additional advanced extensions and improvements

---

## 👥 Team
**Current Team Lead:**  
Tom David Dan Harel

**Current Design Lead:**
Lidor Ben David 

**Current QA Lead:**


**Team Members:**  
Tom David Dan Harel,
Lidor Ben David, 
Yoav Ben Noon,
Omri Halfon,
Samuel Mimoun

---

## 📌 Notes
This repository contains the implementation of **Version 1.0** of the Exam Scheduling System, based on the official software requirements document.

The focus of this version is correctness, modular design, and generation of all valid exam scheduling systems according to the required input and conflict rules.
