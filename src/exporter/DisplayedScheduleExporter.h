#pragma once

#include <string>
#include <vector>

/**
 * @brief Represents one day in the schedule currently displayed in the calendar UI.
 *
 * This struct is intentionally independent from QML and CalendarManager internals.
 * CalendarManager converts its DayInfo objects into this simple export format
 * before sending the data to the exporter.
 */
struct DisplayedScheduleDay {
    /**
     * @brief Date formatted as yyyy-MM-dd.
     */
    std::string date;

    /**
     * @brief Display status of the day.
     *
     * Expected values:
     * 1 = active exam day
     * 2 = excluded day
     * Other values are written as outside/unknown.
     */
    int status;

    /**
     * @brief Optional label shown for this day in the calendar.
     */
    std::string label;
};

/**
 * @brief Data required to export the schedule currently displayed to the user.
 *
 * This DTO keeps the exporter independent from GUI classes, Qt types,
 * and CalendarManager implementation details.
 */
struct DisplayedScheduleExportData {
    /**
     * @brief Human-readable period name, for example "Semester A - Moed A".
     */
    std::string periodName;

    /**
     * @brief Period start date formatted as yyyy-MM-dd.
     */
    std::string startDate;

    /**
     * @brief Period end date formatted as yyyy-MM-dd.
     */
    std::string endDate;

    /**
     * @brief Ordered list of days currently displayed in the calendar.
     */
    std::vector<DisplayedScheduleDay> days;
};

/**
 * @brief Exports the currently displayed calendar schedule into a readable text file.
 *
 * This class belongs to the exporter layer.
 * It does not generate schedules, modify data, or depend on QML.
 */
class DisplayedScheduleExporter {
public:
    /**
     * @brief Writes the displayed schedule data to a text file.
     *
     * @param outputPath Path to the output text file.
     * @param data Prepared schedule data from the GUI/controller layer.
     *
     * @throws std::runtime_error If the file cannot be opened.
     */
    void writeToTextFile(
        const std::string& outputPath,
        const DisplayedScheduleExportData& data
    ) const;
};