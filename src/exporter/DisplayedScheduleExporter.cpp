#include "DisplayedScheduleExporter.h"

#include <fstream>
#include <stdexcept>

void DisplayedScheduleExporter::writeToTextFile(
    const std::string& outputPath,
    const DisplayedScheduleExportData& data
) const {
    // Open the target file for writing.
    // If the file cannot be opened, the caller should know the export failed.
    std::ofstream output(outputPath);

    if (!output) {
        throw std::runtime_error("Could not open output file: " + outputPath);
    }

    // Write a simple human-readable header.
    output << "Displayed Exam Schedule\n";
    output << "=======================\n\n";

    // Write general information about the currently displayed exam period.
    output << "Period: " << data.periodName << "\n";
    output << "Start date: " << data.startDate << "\n";
    output << "End date: " << data.endDate << "\n\n";

    // Write each displayed calendar day with its current status.
    output << "Days:\n";
    output << "-----\n";

    for (const DisplayedScheduleDay& day : data.days) {
        output << day.date << " | ";

        // Convert the internal numeric day status into readable text.
        if (day.status == 1) {
            output << "Active exam day";
        } else if (day.status == 2) {
            output << "Excluded day";
        } else {
            output << "Outside/unknown day";
        }

        // Add the optional label only when it exists.
        if (!day.label.empty()) {
            output << " | " << day.label;
        }

        output << "\n";
    }
}