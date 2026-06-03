#pragma once
#include <string>

class Date {
private:
    int day_;
    int month_;
    int year_;

public:
    Date();
    Date(int day, int month, int year);

    static Date parse(const std::string& text);
    std::string toString() const;

    // Getters 
    int getDay() const;
    int getMonth() const;
    int getYear() const;

    bool isValid() const;
    Date nextDay() const;

    // Comparison operators
    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator>=(const Date& other) const;
};