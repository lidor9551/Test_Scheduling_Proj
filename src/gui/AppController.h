#pragma once

#include <QObject>
#include <QString>
#include <vector>

#include "model/Course.h"
#include "model/ExamPeriod.h"

#include <QVariant>
#include <QVariantList>

class AppController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString coursesFilePath READ coursesFilePath NOTIFY coursesFilePathChanged)
    Q_PROPERTY(QString examPeriodsFilePath READ examPeriodsFilePath NOTIFY examPeriodsFilePathChanged)

    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

    Q_PROPERTY(int courseCount READ courseCount NOTIFY dataChanged)
    Q_PROPERTY(int examPeriodCount READ examPeriodCount NOTIFY dataChanged)
    Q_PROPERTY(bool hasData READ hasData NOTIFY dataChanged)

    Q_PROPERTY(QVariantList courses READ getCoursesVariant NOTIFY dataChanged)
    Q_PROPERTY(QVariantList examPeriods READ getExamPeriodsVariant NOTIFY dataChanged)
public:
    explicit AppController(QObject* parent = nullptr);

    QVariantList getCoursesVariant() const;
    QVariantList getExamPeriodsVariant() const;

    QString coursesFilePath() const;
    QString examPeriodsFilePath() const;

    QString statusMessage() const;
    QString errorMessage() const;

    int courseCount() const;
    int examPeriodCount() const;
    bool hasData() const;

    Q_INVOKABLE void setCoursesFilePath(const QString& path);
    Q_INVOKABLE void setExamPeriodsFilePath(const QString& path);

    Q_INVOKABLE void replaceData();
    Q_INVOKABLE void appendData();
    Q_INVOKABLE void clearMessages();

signals:
    void coursesFilePathChanged();
    void examPeriodsFilePathChanged();

    void statusMessageChanged();
    void errorMessageChanged();

    void dataChanged();

private:
    QString normalizePath(const QString& rawPath) const;

    bool validateSelectedFiles();
    void setStatus(const QString& message);
    void setError(const QString& message);

private:
    QString coursesFilePath_;
    QString examPeriodsFilePath_;

    QString statusMessage_;
    QString errorMessage_;

    std::vector<Course> courses_;
    std::vector<ExamPeriod> examPeriods_;
};