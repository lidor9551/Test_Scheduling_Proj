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

QString CalendarManager::getCurrentStartDate() const {
    if (periods_.empty()) return "";
    const Date& d = periods_[currentPeriodIndex_].getStartDate();
    return QDate(d.getYear(), d.getMonth(), d.getDay()).toString("yyyy-MM-dd");
}

QString CalendarManager::getCurrentEndDate() const {
    if (periods_.empty()) return "";
    const Date& d = periods_[currentPeriodIndex_].getEndDate();
    return QDate(d.getYear(), d.getMonth(), d.getDay()).toString("yyyy-MM-dd");
}

QVariantList CalendarManager::getSemesterList() const {
    QVariantList list;
    for (const ExamPeriod& p : periods_) {
        list.append(QString::fromStdString(p.getSemester() + " - " + p.getMoed()));
    }
    return list;
}

QVariantList CalendarManager::getPeriodTree() const {
    QVariantList tree;
    for (int i = 0; i < (int)periods_.size(); i++) {
        const ExamPeriod& p = periods_[i];
        QVariantMap entry;
        entry["year"]     = p.getStartDate().getYear();
        entry["semester"] = QString::fromStdString(p.getSemester());
        entry["moed"]     = QString::fromStdString(p.getMoed());
        entry["index"]    = i;
        tree.append(entry);
    }
    return tree;
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
    if (periods_.empty()) return;

    QDate newStart = QDate::fromString(newStartStr, "yyyy-MM-dd");
    QDate newEnd   = QDate::fromString(newEndStr,   "yyyy-MM-dd");

    if (!newStart.isValid() || !newEnd.isValid()) {
        qDebug() << ">>> shiftPeriod: invalid dates" << newStartStr << newEndStr;
        return;
    }

    ExamPeriod& p = periods_[currentPeriodIndex_];
    Date start(newStart.day(), newStart.month(), newStart.year());
    Date end(newEnd.day(),     newEnd.month(),   newEnd.year());
    p = ExamPeriod(p.getSemester(), p.getMoed(), start, end, p.getExcludedRanges());

    rebuildDays();
    emit semesterChanged();
    emit daysChanged();
}

void CalendarManager::selectPeriod(int index) {
    if (index < 0 || index >= (int)periods_.size()) return;
    flushDaysToPeriod();
    currentPeriodIndex_ = index;
    rebuildDays();
    emit semesterChanged();
    emit daysChanged();
}

void CalendarManager::nextSemester() {
    if (currentPeriodIndex_ < (int)periods_.size() - 1) {
        flushDaysToPeriod();
        currentPeriodIndex_++;
        rebuildDays();
        emit daysChanged();
        emit semesterChanged();
    }
}

void CalendarManager::previousSemester() {
    if (currentPeriodIndex_ > 0) {
        flushDaysToPeriod();
        currentPeriodIndex_--;
        rebuildDays();
        emit daysChanged();
        emit semesterChanged();
    }
}

void CalendarManager::saveChanges() {
    flushDaysToPeriod();
    qDebug() << ">>> Changes saved for period:" << getCurrentSemester();
    emit daysChanged();
}

void CalendarManager::flushDaysToPeriod() {
    if (periods_.empty()) return;

    ExamPeriod& p = periods_[currentPeriodIndex_];
    std::vector<ExcludedRange> newExclusions;

    for (const DayInfo& d : days_) {
        if (d.status == 2) {
            Date dt(d.date.day(), d.date.month(), d.date.year());
            ExcludedRange r;
            r.start  = dt;
            r.end    = dt;
            r.reason = "manual";
            newExclusions.push_back(r);
        }
    }

    p = ExamPeriod(p.getSemester(), p.getMoed(),
                   p.getStartDate(), p.getEndDate(),
                   newExclusions);
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
        bool isSaturday = (info.date.dayOfWeek() == 6);
        info.status = (excluded || isSaturday) ? 2 : 1;
        days_.push_back(info);
    }
}
