#include "Date.h"
#include "Utils.h" 
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace {
    bool isLeapYear(int year) {
        return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
    }

    int daysInMonth(int month, int year) {
        static const int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        if (month == 2) {
            return isLeapYear(year) ? 29 : 28;
        }
        return days[month - 1];
    }
}

Date::Date() : day_(1), month_(1), year_(1970) {}

Date::Date(int day, int month, int year)
    : day_(day), month_(month), year_(year) {
    if (!isValid()) {
        throw std::invalid_argument("Invalid date");
    }
}

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

std::string Date::toString() const {
    std::ostringstream output;
    output << std::setw(2) << std::setfill('0') << day_ << "-"
           << std::setw(2) << std::setfill('0') << month_ << "-"
           << std::setw(4) << std::setfill('0') << year_;
    return output.str();
}

int Date::getDay() const { return day_; }
int Date::getMonth() const { return month_; }
int Date::getYear() const { return year_; }

bool Date::isValid() const {
    if (year_ < 1 || month_ < 1 || month_ > 12 || day_ < 1) {
        return false;
    }
    return day_ <= daysInMonth(month_, year_);
}

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

bool Date::operator<(const Date& other) const {
    if (year_ != other.year_) return year_ < other.year_;
    if (month_ != other.month_) return month_ < other.month_;
    return day_ < other.day_;
}

bool Date::operator<=(const Date& other) const {
    return *this < other || *this == other;
}

bool Date::operator==(const Date& other) const {
    return day_ == other.day_ && month_ == other.month_ && year_ == other.year_;
}

bool Date::operator!=(const Date& other) const {
    return !(*this == other);
}
bool Date::operator>(const Date& other) const {
    return other < *this;
}

bool Date::operator>=(const Date& other) const {
    return other <= *this;
}