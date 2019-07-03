#ifndef DATA_H
#define DATA_H

#include "calendarcell.h"

#include <QDataStream>
#include <QDate>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QMap>
#include <QPair>
#include <QStandardPaths>
#include <QString>
#include <QVector>

inline namespace Data {
    inline QMap<QString, QVector<QPair<QDate, bool>>> oldVacationData;
    inline QMap<QString, QVector<QPair<QDate, int>>> vacationData;
    inline int startingMonth = 4;

    inline void save() {
        auto appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

        if (!QDir(appDataLocation).exists()) {
            QDir().mkdir(appDataLocation);
        }

        QFile file(appDataLocation + "/vacations.dat");
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "Failed to save data!";
            return;
        }

        QDataStream ds(&file);
        ds.setVersion(QDataStream::Qt_5_0);

        ds << vacationData;

        file.close();
    }

    inline bool load() {
        auto appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QFile file(appDataLocation + "/vacations.dat");
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to load data!";
            return false;
        }

        qDebug() << "Loading new format";
        QDataStream ds(&file);
        ds.setVersion(QDataStream::Qt_5_0);
        vacationData.clear();
        ds >> vacationData;
        file.close();

        // Try loading old format if nothing in new format
        if (vacationData.size() == 0) {
            qDebug() << "Loading old format";
            file.open(QIODevice::ReadOnly);
            QDataStream old_ds(&file);
            old_ds.setVersion(QDataStream::Qt_5_0);
            oldVacationData.clear();
            old_ds >> oldVacationData;

            // Convert old format to new format
            qDebug() << "Loading old save format";
            vacationData.clear();
            foreach(auto key, oldVacationData.keys()) {
                QVector<QPair<QDate, int>> key_data;
                foreach(auto pair, oldVacationData[key]) {
                    key_data.push_back(QPair<QDate, int>(pair.first, int(pair.second)));
                }

                vacationData[key] = key_data;
            }

            file.close();
        }

        return true;
    }

    inline double getVacationCount(QString employee, int year) {
        // Merge unique employee and company holidays
        auto dates = vacationData["Company Holidays"];
        if (employee != "Company Holidays") {
            for (auto d: vacationData[employee]) {
                if (d.second == NONE) { // If date is NONE
                    dates.removeAll(QPair<QDate, int>(d.first, FULL));
                    dates.removeAll(QPair<QDate, int>(d.first, HALF));
                } else if (!dates.contains(d)) { // If date is true and is unique
                    if (d.second == HALF) {
                        dates.removeAll(QPair<QDate, int>(d.first, FULL));
                    }
                    dates.push_back(d);
                }
            }
        }

        // Remove dates for other years
        auto start = QDate(year, startingMonth, 1);
        foreach (auto pair, dates) {
            auto date = pair.first;

            if (date < start || date >= start.addYears(1)) {
                dates.removeAll(QPair<QDate, int>(date, FULL));
                dates.removeAll(QPair<QDate, int>(date, HALF));
                dates.removeAll(QPair<QDate, int>(date, NONE));
            }
        }

        // Separate Full days from half days
        auto fullDates = QVector<QPair<QDate, int>>{};
        auto halfDates = QVector<QPair<QDate, int>>{};
        foreach (auto date, dates) {
            if (date.second == FULL) {
                fullDates.push_back(date);
            } else {
                halfDates.push_back(date);
            }
        }

        return fullDates.count() + double(halfDates.count())/2; // Total holidays taken in current year
    }
};

#endif // DATA_H
