#include "application/SchedulingSession.h"

#include <cassert>
#include <iostream>

int main() {
    SchedulingSession session;

    assert(session.selectedProgramCount() == 0);
    assert(session.selectedPrograms().empty());

    assert(session.selectProgram("83101"));
    assert(session.isProgramSelected("83101"));
    assert(session.selectedProgramCount() == 1);

    assert(!session.selectProgram("83101"));
    assert(session.selectedProgramCount() == 1);

    assert(session.selectProgram("83102"));
    assert(session.selectProgram("83103"));
    assert(session.selectProgram("83104"));
    assert(session.selectProgram("83105"));

    assert(session.selectedProgramCount() == 5);
    assert(!session.selectProgram("83107"));
    assert(session.selectedProgramCount() == 5);

    assert(session.deselectProgram("83103"));
    assert(!session.isProgramSelected("83103"));
    assert(session.selectedProgramCount() == 4);

    assert(session.selectProgram("83107"));
    assert(session.isProgramSelected("83107"));
    assert(session.selectedProgramCount() == 5);

    assert(!session.selectProgram(""));
    assert(!session.deselectProgram("99999"));

    std::cout << "SchedulingSession selected program test passed." << std::endl;
    return 0;
}