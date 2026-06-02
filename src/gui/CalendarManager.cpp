#include "CalendarManager.h"
#include <QDebug>

CalendarManager::CalendarManager(QObject* parent)
    : QObject(parent), currentPeriodIndex_(0) {}

void CalendarManager::setData(QVariantList periods, QVariantList courses)
{
    periods_.clear();
    courses_.clear();

    for (const QVariant& v : periods) {
        if (v.canConvert<ExamPeriod>())
            periods_.push_back(v.value<ExamPeriod>());
    }
    for (const QVariant& v : courses) {
        if (v.canConvert<Course>())
            courses_.push_back(v.value<Course>());
    }

    currentPeriodIndex_ = 0;
    rebuildDays();
    emit semesterChanged();
    emit daysChanged();
}

QVariantList CalendarManager::getDays() const {
    QVariantList result;
    for (const DayInfo& d : days_) {
        QVariantMap map;
        map["date"]   = d.date;
        map["status"] = d.status;
        map["label"]  = d.label;
        result.append(map);
    }
    return result;
}

QString CalendarManager::getCurrentSemester() const {
    if (periods_.empty()) return "";
    return QString::fromStdString(
        periods_[currentPeriodIndex_].getSemester()
        + " - " + periods_[currentPeriodIndex_].getMoed()
    );
}

void CalendarManager::toggleDay(const QString& dateStr) {
    QDate targetDate = QDate::fromString(dateStr, "yyyy-MM-dd");
    if (!targetDate.isValid()) {
        qDebug() << ">>> INVALID DATE:" << dateStr;
        return;
    }
    bool found = false;
    for (DayInfo& d : days_) {
        if (d.date == targetDate) {
            found = true;
            d.status = (d.status == 1) ? 2 : 1;
            break;
        }
    }
    qDebug() << ">>> Date found and modified in C++:" << found;
    emit daysChanged();
}

void CalendarManager::shiftPeriod(const QString& semester,
                                   const QString& newStartStr,
                                   const QString& newEndStr)
{
    qDebug() << ">>> shiftPeriod called:" << newStartStr << "->" << newEndStr;
    if (periods_.empty()) return;

    QDate newStart = QDate::fromString(newStartStr, "yyyy-MM-dd");
    QDate newEnd   = QDate::fromString(newEndStr,   "yyyy-MM-dd");

    ExamPeriod& p = periods_[currentPeriodIndex_];
    Date start(newStart.day(), newStart.month(), newStart.year());
    Date end(newEnd.day(),   newEnd.month(),   newEnd.year());
    p = ExamPeriod(p.getSemester(), p.getMoed(), start, end, p.getExcludedRanges());

    rebuildDays();
    emit daysChanged();
}

void CalendarManager::nextSemester() {
    if (currentPeriodIndex_ < (int)periods_.size() - 1) {
        currentPeriodIndex_++;
        rebuildDays();
        emit daysChanged();
        emit semesterChanged();
    }
}

void CalendarManager::previousSemester() {
    if (currentPeriodIndex_ > 0) {
        currentPeriodIndex_--;
        rebuildDays();
        emit daysChanged();
        emit semesterChanged();
    }
}

void CalendarManager::rebuildDays() {
    days_.clear();
    if (periods_.empty()) return;

    const ExamPeriod& period = periods_[currentPeriodIndex_];
    Date start = period.getStartDate();
    Date end   = period.getEndDate();

    for (Date current = start; current <= end; current = current.nextDay()) {
        DayInfo info;
        info.date  = QDate(current.getYear(), current.getMonth(), current.getDay());
        info.label = "";

        bool excluded = false;
        for (const ExcludedRange& r : period.getExcludedRanges()) {
            if (r.contains(current)) { excluded = true; break; }
        }
        info.status = excluded ? 2 : 1;
        days_.push_back(info);
    }
}