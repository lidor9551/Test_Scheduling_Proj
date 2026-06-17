#include "Date.h"
#include "utils/Utils.h" 
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cmath>

namespace {
    /*
     * Returns true when the given year is a leap year.
     *
     * Leap years are needed because February can have either 28 or 29 days.
     */
    bool isLeapYear(int year) {
        return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
    }

    /*
     * Returns the number of days in a specific month.
     *
     * February is handled separately because it depends on whether the year
     * is a leap year.
     */
    int daysInMonth(int month, int year) {
        static const int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        if (month == 2) {
            return isLeapYear(year) ? 29 : 28;
        }
        return days[month - 1];
    }

    
}

// Creates a default valid date.
Date::Date() : day_(1), month_(1), year_(1970) {}

/*
 * Creates a Date object and validates it immediately.
 *
 * Invalid dates are rejected here, so invalid Date objects do not spread
 * into the rest of the system.
 */
Date::Date(int day, int month, int year)
    : day_(day), month_(month), year_(year) {
    if (!isValid()) {
        throw std::invalid_argument("Invalid date");
    }
}

/*
 * Parses a date from text.
 *
 * The expected input format is DD-MM-YYYY.
 * The input is trimmed first, then split by the '-' character.
 */
Date Date::parse(const std::string& text) {
    std::vector<std::string> parts = Utils::split(Utils::trim(text), '-');
    if (parts.size() != 3) {
        throw std::invalid_argument("Invalid date format: " + text);
    }

    int day = std::stoi(parts[0]);
    int month = std::stoi(parts[1]);
    int year = std::stoi(parts[2]);
    return Date(day, month, year);
}

/*
 * Converts the date into DD-MM-YYYY format.
 *
 * setw and setfill are used so day and month are printed with two digits.
 */
std::string Date::toString() const {
    std::ostringstream output;
    output << std::setw(2) << std::setfill('0') << day_ << "-"
           << std::setw(2) << std::setfill('0') << month_ << "-"
           << std::setw(4) << std::setfill('0') << year_;
    return output.str();
}

// Returns the day part of the date.
int Date::getDay() const { return day_; }

// Returns the month part of the date.
int Date::getMonth() const { return month_; }

// Returns the year part of the date.
int Date::getYear() const { return year_; }

static int toJulianDay(int day, int month, int year) {
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    return day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
}

int Date::daysTo(const Date& other) const {
    int thisJulian = toJulianDay(day_, month_, year_);
    int otherJulian = toJulianDay(other.getDay(), other.getMonth(), other.getYear());
    
    return std::abs(thisJulian - otherJulian);
}

/*
 * Checks whether the stored values form a real calendar date.
 *
 * This protects against impossible dates such as:
 * - month 13
 * - day 0
 * - February 30
 */
bool Date::isValid() const {
    if (year_ < 1 || month_ < 1 || month_ > 12 || day_ < 1) {
        return false;
    }
    return day_ <= daysInMonth(month_, year_);
}

/*
 * Returns the next calendar day.
 *
 * This method handles:
 * - moving from the end of a month to the next month
 * - moving from December to January of the next year
 * - February in leap years
 */
Date Date::nextDay() const {
    int day = day_ + 1;
    int month = month_;
    int year = year_;

    if (day > daysInMonth(month, year)) {
        day = 1;
        ++month;
    }

    if (month > 12) {
        month = 1;
        ++year;
    }

    return Date(day, month, year);
}

/*
 * Compares dates chronologically.
 *
 * The comparison checks year first, then month, then day.
 */
bool Date::operator<(const Date& other) const {
    if (year_ != other.year_) return year_ < other.year_;
    if (month_ != other.month_) return month_ < other.month_;
    return day_ < other.day_;
}

// Returns true when this date is earlier than or equal to another date.
bool Date::operator<=(const Date& other) const {
    return *this < other || *this == other;
}

// Returns true when both dates contain the same day, month, and year.
bool Date::operator==(const Date& other) const {
    return day_ == other.day_ && month_ == other.month_ && year_ == other.year_;
}

// Returns true when the two dates are not equal.
bool Date::operator!=(const Date& other) const {
    return !(*this == other);
}

// Returns true when this date is later than another date.
bool Date::operator>(const Date& other) const {
    return other < *this;
}

// Returns true when this date is later than or equal to another date.
bool Date::operator>=(const Date& other) const {
    return other <= *this;
}