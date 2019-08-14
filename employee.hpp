#ifndef EMPLOYEE_HPP
#define EMPLOYEE_HPP

#include <QDate>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QMap>

class Employee {
public:
    explicit Employee() {

    }

    const QString &getName() {
        return name;
    }

    void setName(const QString &name) {
        this->name = name;
    }

    const QDate &getStartDate() {
        return startDate;
    }

    void setStartDate(const QDate &date) {
        this->startDate = date;
    }

    QMap<QDate, int> getVacationData() {
        return vacationData;
    }

    void setVacationData(QMap<QDate, int> &vacationData) {
        this->vacationData = vacationData;
    }

    void read(const QJsonObject &json) {
        const auto name = json["name"].toString();
        const auto startDate = QDate::fromString(json["startDate"].toString());
        QMap<QDate, int> vacationMap;
        for (auto vacation : json["vacationData"].toArray()) {
            vacationMap[QDate::fromString(vacation.toObject()["date"].toString())] = vacation.toObject()["status"].toInt();
        }

        setStartDate(startDate);
        setName(name);
        setVacationData(vacationMap);
    }

    void write(QJsonObject &json) {
        json["name"] = name;
        json["startDate"] = startDate.toString();

        QJsonArray vacations;
        for (auto date : vacationData.keys()) {
            QJsonObject data;
            data["date"] = date.toString();
            data["status"] = vacationData[date];
            vacations.push_back(data);
        }
        json["vacationData"] = vacations;
    }
private:
    QString name;
    QDate startDate;
    QMap<QDate, int> vacationData = {};
};

#endif // EMPLOYEE_HPP
