#ifndef DATE_H
#define DATE_H

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

    int day() const;
    int month() const;
    int year() const;

    bool isValid() const;
    Date nextDay() const;

    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
};

#endif
