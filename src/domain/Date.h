#pragma once
#include <string>

/*
 * Date is a small domain value object used by the scheduling system.
 *
 * The project uses this custom Date class in the domain and scheduling layers
 * instead of relying directly on Qt types such as QDate.
 *
 * This keeps the core business logic independent from the UI framework.
 */
class Date {
private:
    // Day of the month, starting from 1.
    int day_;

    // Month number, from 1 to 12.
    int month_;

    // Full year value, for example 2026.
    int year_;

public:
    // Creates a default valid date.
    Date();

    // Creates a date from day, month, and year values.
    // The implementation validates the date immediately.
    Date(int day, int month, int year);

    // Parses a date from text.
    // The expected format is DD-MM-YYYY.
    static Date parse(const std::string& text);

    // Converts the date back into a readable string format.
    std::string toString() const;

    // Getters 
    // Returns the day part of the date.
    int getDay() const;

    // Returns the month part of the date.
    int getMonth() const;

    // Returns the year part of the date.
    int getYear() const;

    // Returns true if the current day/month/year combination is a real calendar date.
    bool isValid() const;

    // Returns the absolute number of days between this date and another date
    int daysTo(const Date& other) const;

    // Returns the next calendar day.
    Date nextDay() const;

    // Comparison operators
    // Allows dates to be sorted and compared by chronological order.
    bool operator<(const Date& other) const;

    // Returns true when this date is earlier than or equal to another date.
    bool operator<=(const Date& other) const;

    // Returns true when both dates have the same day, month, and year.
    bool operator==(const Date& other) const;

    // Returns true when the dates are different.
    bool operator!=(const Date& other) const;

    // Returns true when this date is later than another date.
    bool operator>(const Date& other) const;

    // Returns true when this date is later than or equal to another date.
    bool operator>=(const Date& other) const;
};